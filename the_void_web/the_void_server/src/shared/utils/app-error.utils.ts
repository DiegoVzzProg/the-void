export class AppError extends Error {
    public statusCode: number;
    public name: string;

    constructor(message: string, statusCode: number = 500, name: string = "Error") {
        super(message);
        this.statusCode = statusCode;
        this.name = name;

        Error.captureStackTrace(this, this.constructor);
    }
}