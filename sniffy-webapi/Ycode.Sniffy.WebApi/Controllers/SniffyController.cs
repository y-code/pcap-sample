using System.Net;
using Microsoft.AspNetCore.Mvc;
using Ycode.Sniffy.Domain;

[ApiController]
[Route("[controller]")]
public class SniffyController : Controller {
    private readonly SniffyDbContext _db;

    public SniffyController(SniffyDbContext db) {
        _db = db;
    }
    [HttpGet("[action]")]
    public ActionResult Graph() {
        return Ok(new
            {
                endpointVertices = _db.Set<EndpointModel>()
                    .AsEnumerable()
                    .Select(x => new {
                        x.Id,
                        x.Version,
                        Address = new IPAddress(x.Address).ToString(),
                    }),
                ProtocolClientVertices = _db.Set<ProtocolClientModel>()
                    .AsEnumerable()
                    .Select(x => new {
                        x.Id,
                        x.Version,
                        Address = new IPAddress(x.Address).ToString(),
                        x.ServerPort,
                        x.Ports,
                    }),
                ProtocolServerVertices = _db.Set<ProtocolServerModel>()
                    .AsEnumerable()
                    .Select(x => new {
                        x.Id,
                        x.Version,
                        Address = new IPAddress(x.Address).ToString(),
                        x.Port,
                    }),
                EndpointProtocolClientEdges = _db.Set<EndpointProtocolClientModel>()
                    // .Include(x => x.Start)
                    // .Include(x => x.End)
                    .AsEnumerable(),
                EndpointProtocolServerEdges = _db.Set<EndpointProtocolServerModel>()
                    // .Include(x => x.Start)
                    // .Include(x => x.End)
                    .AsEnumerable(),
                TrafficEdges = _db.Set<TrafficModel>()
                    // .Include(x => x.Start)
                    // .Include(x => x.End)
                    .AsEnumerable(),
            });
    }
}
