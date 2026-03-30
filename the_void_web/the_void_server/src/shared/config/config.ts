export class Config {
    public static readonly SALT_ROUNDS = parseInt(process.env.SALT_ROUNDS || "10");
}