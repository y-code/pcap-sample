using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.Options;
using Ycode.Sniffy.WebApi;

namespace Ycode.Sniffy.Domain;

public class SniffyDbContext : DbContext
{
    public string Schema { get; init; }

    public SniffyDbContext(DbContextOptions options, IOptions<SniffyWebApiConfig> config) : base(options)
    {
        Schema = config.Value.GraphSchema;
    }

    protected override void OnModelCreating(ModelBuilder builder) {
        base.OnModelCreating(builder);

        builder.Entity<TrafficMetrics>()
            .HasNoKey();

        builder.Entity<EndpointModel>()
            .ToView("v_endpoint", Schema);

        builder.Entity<ProtocolClientModel>()
            .ToView("v_protocol_client", Schema);

        builder.Entity<ProtocolServerModel>()
            .ToView("v_protocol_server", Schema);

        {
            var entity = builder.Entity<EndpointProtocolClientModel>()
                .ToView("v_endpoint_protocol_client", Schema);
            entity.Navigation(e => e.Start).AutoInclude(false);
            entity.Navigation(e => e.End).AutoInclude(false);
        }

        {
            var entity = builder.Entity<EndpointProtocolServerModel>()
                .ToView("v_endpoint_protocol_server", Schema);
            entity.Navigation(e => e.Start).AutoInclude(false);
            entity.Navigation(e => e.End).AutoInclude(false);
        }

        {
            var entity = builder.Entity<TrafficModel>()
                .ToView("v_traffic", Schema);
            entity.Navigation(e => e.Start).AutoInclude(false);
            entity.Navigation(e => e.End).AutoInclude(false);
        }
    }
}
