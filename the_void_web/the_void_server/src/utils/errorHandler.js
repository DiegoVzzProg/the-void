/**
 * Global Error Handler for Fastify
 * Standardizes error responses across the API.
 */
function errorHandler(error, request, reply) {
  // Log del error para debugging (usando el logger de Fastify)
  request.log.error(error);

  let statusCode = error.statusCode || 500;
  let response = {
    error: error.name || "Internal Server Error",
    message: error.message || "An unexpected error occurred",
  };

  // Manejo específico de errores de Prisma
  if (error.code) {
    switch (error.code) {
      case "P2002": // Unique constraint failed
        statusCode = 409;
        response.error = "Conflict";
        response.message = "Resource already exists (Unique constraint failed)";
        break;
      case "P2025": // Record not found
        statusCode = 404;
        response.error = "Not Found";
        response.message = "Resource not found";
        break;
    }
  }

  // Errores de validación de Fastify (Schema)
  if (error.validation) {
    statusCode = 400;
    response.error = "Bad Request";
    response.message = "Validation failed";
    response.details = error.validation; // Detalles útiles para el frontend
  }

  reply.code(statusCode).send(response);
}

module.exports = errorHandler;
