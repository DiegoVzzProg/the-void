using the_void_ui.Client.Models;
using System.Net.Http.Json;
using System.Text.Json;

namespace the_void_ui.Client.Services
{
    public class ArticulosService
    {
        private readonly HttpClient _http;

        public ArticulosService(HttpClient http)
        {
            _http = http;
        }

        // Método para llamar a tu endpoint específico
        public async Task<Dictionary<string, JsonElement>?> BuscarArticulosAsync(ArticulosModel filtros)
        {
            // 1. Hacemos el POST a la ruta que corregimos
            var response = await _http.PostAsJsonAsync("api/Articulos/BuscarArticulosVentasExistenciasNuevo", filtros);

            // 2. Si falla, puedes manejar el error aquí o devolver null
            if (!response.IsSuccessStatusCode)
            {
                // Opcional: Leer el error con response.Content.ReadAsStringAsync();
                return null;
            }

            // 3. Deserializamos la respuesta dinámica
            // Usamos Dictionary<string, JsonElement> para manejar "Tabla0", "Tabla1", etc.
            return await response.Content.ReadFromJsonAsync<Dictionary<string, JsonElement>>();
        }
    }
}
