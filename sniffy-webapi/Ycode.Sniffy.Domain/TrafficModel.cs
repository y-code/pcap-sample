using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace Ycode.Sniffy.Domain;

public class EndpointModel {
    [Column("id")]
    [Key()]
    public long Id { get; set;}
    [Column("version")]
    public short Version { get; set; }
    [Column("address")]
    public byte[] Address { get; set; } = Array.Empty<byte>();
}

public class ProtocolClientModel {
    [Column("id")]
    [Key()]
    public long Id { get; set;}
    [Column("version")]
    public short Version { get; set; }
    [Column("address")]
    public byte[] Address { get; set; } = Array.Empty<byte>();
    [Column("server_port")]
    public int ServerPort { get; set; }
    [Column("ports")]
    public int[] Ports { get; set; }
}

public class ProtocolServerModel {
    [Column("id")]
    [Key()]
    public long Id { get; set;}
    [Column("version")]
    public short Version { get; set; }
    [Column("address")]
    public byte[] Address { get; set; } = Array.Empty<byte>();
    [Column("port")]
    public int Port { get; set; }
}

public class EndpointProtocolClientModel {
    [Column("id")]
    [Key()]
    public long Id { get; set;}
    [Column("start")]
    [ForeignKey(nameof(Start))]
    public long StartId { get; set; }
    public EndpointModel? Start { get; set; }
    [Column("end")]
    [ForeignKey(nameof(End))]
    public long EndId { get; set; }
    public ProtocolClientModel? End { get; set; }
}

public class EndpointProtocolServerModel {
    [Column("id")]
    [Key()]
    public long Id { get; set;}
    [Column("start")]
    [ForeignKey(nameof(Start))]
    public long StartId { get; set; }
    public EndpointModel? Start { get; set; }
    [Column("end")]
    [ForeignKey(nameof(End))]
    public long EndId { get; set; }
    public ProtocolServerModel? End { get; set; }
}

public class TrafficModel {
    [Column("id")]
    [Key()]
    public long Id { get; set;}
    [Column("start")]
    [ForeignKey(nameof(Start))]
    public long StartId { get; set; }
    public ProtocolClientModel? Start { get; set; }
    [Column("end")]
    [ForeignKey(nameof(End))]
    public long EndId { get; set; }
    public ProtocolServerModel? End { get; set; }
    [Column("up_size")]
    public long UpSize { get; set; }
    [Column("down_size")]
    public long DownSize { get; set; }
}
