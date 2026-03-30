import { FastifyError, FastifyReply, FastifyRequest } from "fastify";
import { AppError } from "@/shared/utils/app-error.utils";

const errorHandler = (
  error: FastifyError | AppError | Error | any,
  request: FastifyRequest,
  reply: FastifyReply
) => {
  // 1. Handle our custom AppError (Errores seguros que tú creaste)
  if (error instanceof AppError) {
    return reply.status(error.statusCode).send({
      success: false,
      message: error.message,
      error: error.name
    });
  }

  // 2. Handle Zod validation errors
  if (error.validation) {
    return reply.status(400).send({
      success: false,
      message: "Validation Error",
      errors: error.validation
    });
  }

  // 3. Handle Prisma Errors (¡CORREGIDO!)
  // Prisma puede guardar el código en 'code' o en 'errorCode' cuando la BD se cae
  const prismaCode = error.code || error.errorCode;

  if (prismaCode && typeof prismaCode === 'string' && prismaCode.startsWith('P')) {
    console.error(`[Prisma DB Error ${prismaCode}]:`, error.message);

    switch (prismaCode) {
      case "P2002":
        return reply.status(409).send({
          success: false,
          message: "The resource already exists.",
          error: "Conflict"
        });
      case "P2025":
        return reply.status(404).send({
          success: false,
          message: "Resource not found.",
          error: "NotFound"
        });
      case "P1001": // Ahora sí detectará cuando localhost:3306 esté apagado
        return reply.status(500).send({
          success: false,
          message: "Service temporarily unavailable. Please try again later.",
          error: "DatabaseConnectionError"
        });
      default:
        return reply.status(500).send({
          success: false,
          message: "Internal Server Error",
          error: "InternalServerError"
        });
    }
  }

  // 4. Handle other Fastify errors (¡EL BLOQUE DE LA FUGA CORREGIDO!)
  if (error.statusCode) {
    // Verificamos si Fastify nos mandó un error 500 o superior
    const isServerError = error.statusCode >= 500;

    // Imprimimos el error real en tu consola para que no lo pierdas de vista
    if (isServerError) {
      console.error(`[Server Error ${error.statusCode}]:`, error.message);
    }

    return reply.status(error.statusCode).send({
      success: false,
      // Si es un error 500, le mentimos al cliente. Si es 400, dejamos pasar el mensaje.
      message: isServerError ? "Internal Server Error" : error.message,
      error: error.name || "Error"
    });
  }

  // 5. Fallback for unhandled unexpected errors
  console.error("[Unhandled Error]:", error);
  return reply.status(500).send({
    success: false,
    message: "Internal Server Error",
    error: "InternalServerError"
  });
};

export default errorHandler;