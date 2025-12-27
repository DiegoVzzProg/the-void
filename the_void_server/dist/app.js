import express from 'express';
const app = express();
const port = 3000;
// Middleware para que entienda JSON (útil para APIs)
app.use(express.json());
// Ruta principal
app.get('/', (req, res) => {
    res.send('¡Éxito! Tu API con TypeScript y Visual Studio funciona 🚀');
});
// Ruta de prueba JSON
app.get('/ping', (req, res) => {
    res.json({ mensaje: 'pong', fecha: new Date() });
});
app.listen(port, () => {
    console.log(`--- Servidor corriendo en http://localhost:${port} ---`);
});
//# sourceMappingURL=app.js.map