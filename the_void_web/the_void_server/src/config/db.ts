import dotenv from "dotenv";
import mysql from "mysql2/promise";

dotenv.config();

const pool = mysql.createPool({
  host: process.env.DB_HOST || "localhost",
  user: process.env.DB_USER || "root",
  password: process.env.DB_PASSWORD || "",
  database: process.env.DB_NAME || "the_void_db",
  port: Number(process.env.DB_PORT) || 3306,
  waitForConnections: true,
  connectionLimit: 10,
  queueLimit: 0,
});

const checkConnection = async () => {
  try {
    const connection = await pool.getConnection();
    console.log("[DB] Conexión a MySQL exitosa");
    connection.release();
  } catch (error) {
    console.error("[DB FATAL] No se pudo conectar a MySQL:", (error as Error).message);
    process.exit(1);
  }
};

checkConnection();

export default pool;
