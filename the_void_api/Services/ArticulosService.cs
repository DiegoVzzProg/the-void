using the_void_api.Data;
using the_void_api.Interfaces;
using the_void_api.Models;
using Microsoft.AspNetCore.Mvc;
using System.Data;

namespace the_void_api.Services
{
    public class ArticulosService : IArticulosService
    {
        private readonly IRunDataService _sql;
        public ArticulosService(IRunDataService sql)
        {
            _sql = sql;
        }
        public async Task<object> sp_buscar_articulo_ventas_existencias_nuevo(ArticulosModel model)
        {
            var datos = await _sql.Exec("sp_buscar_articulo_ventas_existencias_nuevo", model);

            if (!datos.ContainsKey("Tabla0")) return null;

            return datos;
        }
    }
}
