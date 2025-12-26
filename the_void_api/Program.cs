
using System.Net.WebSockets;
using System.Text;
using the_void_api.Data;
using the_void_api.Interfaces;
using the_void_api.Services;
using static the_void_api.Models.ProgramModel;

namespace the_void_api
{
    public class Program
    {
        public static void Main(string[] args)
        {
            var builder = WebApplication.CreateBuilder(args);

            // Add services to the container.

            #region Servicios
            builder.Services.AddScoped<IRunDataService, RunDataService>();
            builder.Services.AddScoped<IArticulosService, ArticulosService>();
            #endregion

            builder.Services.AddControllers();
            // Learn more about configuring OpenAPI at https://aka.ms/aspnet/openapi
            builder.Services.AddOpenApi();

            builder.Services.AddCors(options =>
            {
                options.AddPolicy("AllowBlazor", policy =>
                {
                    policy.AllowAnyOrigin()
                          .AllowAnyMethod()
                          .AllowAnyHeader();
                });
            });

            var app = builder.Build();

            // Configure the HTTP request pipeline.
            if (app.Environment.IsDevelopment())
            {
                app.MapOpenApi();
            }

            app.UseHttpsRedirection();
            app.UseCors("AllowBlazor");


            app.UseAuthorization();


            app.MapControllers();

            app.UseWebSockets();
            WebSocket? clientSocket = null;

            app.Map("/ws", async (HttpContext context) =>
            {
                if (context.WebSockets.IsWebSocketRequest)
                {
                    using var webSocket = await context.WebSockets.AcceptWebSocketAsync();
                    clientSocket = webSocket; // Guardamos la conexión
                    Console.WriteLine("[API] Cliente C++ Conectado.");

                    // Mantener la conexión viva esperando cierre
                    var buffer = new byte[1024 * 4];
                    while (webSocket.State == WebSocketState.Open)
                    {
                        await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);
                    }
                    clientSocket = null;
                }
                else
                {
                    context.Response.StatusCode = 400;
                }
            });

            // 2. Endpoint para recibir comandos desde Blazor
            // Endpoint para recibir comandos desde Blazor
            app.MapPost("/api/command", async (CommandPayload cmd) =>
            {
                Console.WriteLine($"[API DEBUG] 1. Intento de comando recibido: {cmd.Module}");

                if (clientSocket == null)
                {
                    Console.WriteLine("[API ERROR] La variable clientSocket es NULL.");
                    return Results.Problem("Error interno: Socket nulo");
                }

                Console.WriteLine($"[API DEBUG] 2. Estado del Socket: {clientSocket.State}");

                if (clientSocket.State == WebSocketState.Open)
                {
                    var json = System.Text.Json.JsonSerializer.Serialize(cmd);
                    var bytes = Encoding.UTF8.GetBytes(json);

                    await clientSocket.SendAsync(
                        new ArraySegment<byte>(bytes),
                        WebSocketMessageType.Text,
                        true,
                        CancellationToken.None);

                    Console.WriteLine($"[API SUCCESS] 3. Enviado al C++ ({bytes.Length} bytes)");
                    return Results.Ok(new { status = "Enviado" });
                }
                else
                {
                    Console.WriteLine("[API ERROR] El socket existe pero NO está abierto.");
                    return Results.Problem("Cliente C++ desconectado");
                }
            });

            app.Run();
        }
    }
}
