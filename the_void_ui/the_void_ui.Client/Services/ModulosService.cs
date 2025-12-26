using System.Net.Http.Json;
using System.Text.Json;
using the_void_ui.Client.Models;

namespace the_void_ui.Client.Services
{
    public class ModulosService
    {
        private readonly HttpClient _http;

        public ModulosService(HttpClient http)
        {
            _http = http;
        }

        public async Task<string?> Module(ModulosModel command)
        {
            var response = await _http.PostAsJsonAsync("api/command", command);

            if (!response.IsSuccessStatusCode)
            {
                return null;
            }

            return "Ok";
        }
    }
}
