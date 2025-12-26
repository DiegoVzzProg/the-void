using the_void_api.Models;
using Microsoft.AspNetCore.Mvc;
using System.Data;

namespace the_void_api.Interfaces
{
    public interface IArticulosService
    {
        public Task<object> sp_buscar_articulo_ventas_existencias_nuevo(ArticulosModel model);
    }
}
