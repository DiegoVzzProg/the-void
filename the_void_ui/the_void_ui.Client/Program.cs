using Microsoft.AspNetCore.Components.WebAssembly.Hosting;
using the_void_ui.Client.Services;

namespace the_void_ui.Client
{
    internal class Program
    {
        static async Task Main(string[] args)
        {
            var builder = WebAssemblyHostBuilder.CreateDefault(args);

            builder.Services.AddScoped(sp => new HttpClient { BaseAddress = new Uri("http://localhost:5010") });

            #region Services
            builder.Services.AddScoped<ArticulosService>();
            #endregion

            await builder.Build().RunAsync();
        }
    }
}
