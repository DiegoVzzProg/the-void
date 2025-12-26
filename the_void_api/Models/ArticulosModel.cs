using the_void_api.Interfaces;

namespace the_void_api.Models
{
    public class ArticulosModel
    {
        public required string pvalor { get; set; }
        public int pidc_sucursal { get; set; } = 0;
        public required int pidc_usuario { get; set; }
        public int pmin { get; set; } = 0;
        public int pmax { get; set; } = 0;
        public int pagina { get; set; } = 0;
        public int idc_cotizacion { get; set; } = 0;
        public string pcadena { get; set; } = string.Empty;
        public int ptot { get; set; } = 0;
        public string pmarca {get; set;} = string.Empty;
        public int? pidc_listap {get; set;}
        public bool predondear { get; set; } = false;
        public bool pexistencias {get; set;} = false;
        public bool pofertas { get; set; } = false;
    }
}
