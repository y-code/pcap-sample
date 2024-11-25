using System;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace Ycode.Sniffy.Domain.Migrations
{
    /// <inheritdoc />
    public partial class add_tbl_traffic_metrics : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.CreateTable(
                name: "traffic_metrics",
                columns: table => new
                {
                    timestamp = table.Column<DateTimeOffset>(type: "timestamp with time zone", nullable: false),
                    up_version = table.Column<short>(type: "smallint", nullable: false),
                    up_address = table.Column<byte[]>(type: "bytea", nullable: false),
                    down_version = table.Column<short>(type: "smallint", nullable: false),
                    down_address = table.Column<byte[]>(type: "bytea", nullable: false),
                    up_port = table.Column<int>(type: "integer", nullable: false),
                    down_ports = table.Column<int[]>(type: "integer[]", nullable: false),
                    up_size = table.Column<long>(type: "bigint", nullable: false),
                    down_size = table.Column<long>(type: "bigint", nullable: false)
                },
                constraints: table =>
                {
                });
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "traffic_metrics");
        }
    }
}
