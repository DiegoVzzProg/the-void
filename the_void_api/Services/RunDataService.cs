using the_void_api.Interfaces;
using Dapper;
using Microsoft.Data.SqlClient;
using Microsoft.Extensions.Logging;
using System.Data;

namespace the_void_api.Data
{
    public class RunDataService : IRunDataService
    {
        private readonly IConfiguration _configuration;
        private readonly string? _connectionString;
        private readonly ILogger<RunDataService> _logger;
        public RunDataService(IConfiguration configuration, ILogger<RunDataService> logger)
        {
            _configuration = configuration;
            _connectionString = _configuration.GetConnectionString("ConexionSQL");
            _logger = logger;
        }

        public async Task<Dictionary<string, IEnumerable<dynamic>>> Exec(string store, object? parametros = null)
        {
            var resultado = new Dictionary<string, IEnumerable<dynamic>>();
            try
            {
                using (var connection = new SqlConnection(_connectionString))
                {
                    if (connection.State != ConnectionState.Open)
                        await connection.OpenAsync();

                    using (var multi = await connection.QueryMultipleAsync(store, parametros, commandType: CommandType.StoredProcedure))
                    {
                        int contadorTablas = 0;

                        while (!multi.IsConsumed)
                        {
                            try
                            {
                                var tabla = await multi.ReadAsync<dynamic>();
                                resultado.Add($"Tabla{contadorTablas}", tabla);
                                contadorTablas++;
                            }
                            catch (Exception ex)
                            {
                                _logger.LogWarning($"Fin de lectura de tablas o error menor leyendo tabla {contadorTablas}: {ex.Message}");
                                break;
                            }
                        }
                    }
                }

                return resultado;
            }
            catch (SqlException sqlEx)
            {
                _logger.LogError(sqlEx, $"Error CRÍTICO de SQL al ejecutar {store}. Error: {sqlEx.Message}");
                throw;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error general en SqlDataService ejecutando {store}");
                throw;
            }
        }
    }
}
