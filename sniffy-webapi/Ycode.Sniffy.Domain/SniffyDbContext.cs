using Microsoft.EntityFrameworkCore;

namespace Ycode.Sniffy.Domain;

public class SniffyDbContext : DbContext
{
    public string Schema { get; init; } = "traffic_graph_31";

    public SniffyDbContext(DbContextOptions options) : base(options) {}

    protected override void OnModelCreating(ModelBuilder builder) {
        base.OnModelCreating(builder);

        builder.Entity<EndpointModel>()
            .ToTable("v_endpoint", Schema);

        builder.Entity<ProtocolClientModel>()
            .ToTable("v_protocol_client", Schema);

        builder.Entity<ProtocolServerModel>()
            .ToTable("v_protocol_server", Schema);

        {
            var entity = builder.Entity<EndpointProtocolClientModel>()
                .ToTable("v_endpoint_protocol_client", Schema);
            entity.Navigation(e => e.Start).AutoInclude(false);
            entity.Navigation(e => e.End).AutoInclude(false);
        }

        {
            var entity = builder.Entity<EndpointProtocolServerModel>()
                .ToTable("v_endpoint_protocol_server", Schema);
            entity.Navigation(e => e.Start).AutoInclude(false);
            entity.Navigation(e => e.End).AutoInclude(false);
        }

        {
            var entity = builder.Entity<TrafficModel>()
                .ToTable("v_traffic", Schema);
            entity.Navigation(e => e.Start).AutoInclude(false);
            entity.Navigation(e => e.End).AutoInclude(false);
        }
    }
}
