namespace the_void_api.Interfaces
{
    public interface IRunDataService
    {
        Task<Dictionary<string, IEnumerable<dynamic>>> Exec(string store, object? parametros = null);
    }
}
