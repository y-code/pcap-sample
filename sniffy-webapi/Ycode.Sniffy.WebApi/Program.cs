using Ycode.Sniffy.Domain;
using Microsoft.EntityFrameworkCore;
using Ycode.Sniffy.WebApi;

const string CORS_POLICY_FRONTEND = nameof(CORS_POLICY_FRONTEND);

var builder = WebApplication.CreateBuilder(args);

builder.Services.Configure<SniffyWebApiConfig>(builder.Configuration);

builder.Services.AddDbContext<SniffyDbContext>((serviceProvider, options) => {
    options.UseNpgsql(builder.Configuration.GetConnectionString("SniffyDb"));
});
builder.Services.AddControllers();
builder.Services.AddCors(options => {
  options.AddPolicy(name: CORS_POLICY_FRONTEND, policy => {
        policy.WithOrigins("http://localhost:4200");
    });
});

// Add services to the container.
// Learn more about configuring Swagger/OpenAPI at https://aka.ms/aspnetcore/swashbuckle
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

var app = builder.Build();

// Configure the HTTP request pipeline.
if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}

app.UseHttpsRedirection();

app.UseRouting();
app.UseCors(CORS_POLICY_FRONTEND);
app.MapGroup("/api").MapControllerRoute(
    name: "default",
    pattern: "{controller=Sniffy}/{action=Index}/{id?}"
);

var summaries = new[]
{
    "Freezing", "Bracing", "Chilly", "Cool", "Mild", "Warm", "Balmy", "Hot", "Sweltering", "Scorching"
};

app.MapGet("/weatherforecast", () =>
{
    var forecast =  Enumerable.Range(1, 5).Select(index =>
        new WeatherForecast
        (
            DateOnly.FromDateTime(DateTime.Now.AddDays(index)),
            Random.Shared.Next(-20, 55),
            summaries[Random.Shared.Next(summaries.Length)]
        ))
        .ToArray();
    return forecast;
})
.WithName("GetWeatherForecast")
.WithOpenApi();

app.Run();

record WeatherForecast(DateOnly Date, int TemperatureC, string? Summary)
{
    public int TemperatureF => 32 + (int)(TemperatureC / 0.5556);
}
