using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Design;
using Microsoft.Extensions.Options;
using Ycode.Sniffy.WebApi;

namespace Ycode.Sniffy.Domain;

public class DbContextFactory : IDesignTimeDbContextFactory<SniffyDbContext>
{
    public SniffyDbContext CreateDbContext(string[] args)
    {
        var builder = new DbContextOptionsBuilder<SniffyDbContext>();
        builder.UseNpgsql("Host=localhost;Port=5432;Database=sniffy;Username=postgres;Password=postgres");
        return new SniffyDbContext(builder.Options, new OptionsWrapper<SniffyWebApiConfig>(new()));
    }
}
