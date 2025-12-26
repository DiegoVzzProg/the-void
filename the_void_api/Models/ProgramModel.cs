namespace the_void_api.Models
{
    public class ProgramModel
    {
        public record CommandPayload(string Module, bool Enabled, double Value);
    }
}
