using the_void_api.Interfaces;
using the_void_api.Models;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;

namespace the_void_api.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class ArticulosController : ControllerBase
    {
        private readonly IArticulosService _articulosService;

        public ArticulosController(IArticulosService articulosService)
        {
            _articulosService = articulosService;
        }


        [HttpPost("BuscarArticulosVentasExistenciasNuevo")]
        public async Task<IActionResult> sp_buscar_articulo_ventas_existencias_nuevo([FromBody] ArticulosModel data)
        {
            try
            {
                var resultado = await _articulosService.sp_buscar_articulo_ventas_existencias_nuevo(data);

                return Ok(resultado);
            }
            catch (Exception ex)
            {
                return StatusCode(599, "Ocurrió un error interno al procesar el reporte.");
            }
        }
    }
}
