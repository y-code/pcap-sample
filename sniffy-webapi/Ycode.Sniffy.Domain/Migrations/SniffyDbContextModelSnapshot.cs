﻿// <auto-generated />
using System;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Infrastructure;
using Microsoft.EntityFrameworkCore.Storage.ValueConversion;
using Npgsql.EntityFrameworkCore.PostgreSQL.Metadata;
using Ycode.Sniffy.Domain;

#nullable disable

namespace Ycode.Sniffy.Domain.Migrations
{
    [DbContext(typeof(SniffyDbContext))]
    partial class SniffyDbContextModelSnapshot : ModelSnapshot
    {
        protected override void BuildModel(ModelBuilder modelBuilder)
        {
#pragma warning disable 612, 618
            modelBuilder
                .HasAnnotation("ProductVersion", "9.0.0")
                .HasAnnotation("Relational:MaxIdentifierLength", 63);

            NpgsqlModelBuilderExtensions.UseIdentityByDefaultColumns(modelBuilder);

            modelBuilder.Entity("Ycode.Sniffy.Domain.EndpointModel", b =>
                {
                    b.Property<long>("Id")
                        .HasColumnType("bigint")
                        .HasColumnName("id");

                    b.Property<byte[]>("Address")
                        .IsRequired()
                        .HasColumnType("bytea")
                        .HasColumnName("address");

                    b.Property<short>("Version")
                        .HasColumnType("smallint")
                        .HasColumnName("version");

                    b.HasKey("Id");

                    b.ToTable((string)null);

                    b.ToView("v_endpoint", "traffic_graph_31");
                });

            modelBuilder.Entity("Ycode.Sniffy.Domain.EndpointProtocolClientModel", b =>
                {
                    b.Property<long>("Id")
                        .HasColumnType("bigint")
                        .HasColumnName("id");

                    b.Property<long>("EndId")
                        .HasColumnType("bigint")
                        .HasColumnName("end");

                    b.Property<long>("StartId")
                        .HasColumnType("bigint")
                        .HasColumnName("start");

                    b.HasKey("Id");

                    b.HasIndex("EndId");

                    b.HasIndex("StartId");

                    b.ToTable((string)null);

                    b.ToView("v_endpoint_protocol_client", "traffic_graph_31");
                });

            modelBuilder.Entity("Ycode.Sniffy.Domain.EndpointProtocolServerModel", b =>
                {
                    b.Property<long>("Id")
                        .HasColumnType("bigint")
                        .HasColumnName("id");

                    b.Property<long>("EndId")
                        .HasColumnType("bigint")
                        .HasColumnName("end");

                    b.Property<long>("StartId")
                        .HasColumnType("bigint")
                        .HasColumnName("start");

                    b.HasKey("Id");

                    b.HasIndex("EndId");

                    b.HasIndex("StartId");

                    b.ToTable((string)null);

                    b.ToView("v_endpoint_protocol_server", "traffic_graph_31");
                });

            modelBuilder.Entity("Ycode.Sniffy.Domain.ProtocolClientModel", b =>
                {
                    b.Property<long>("Id")
                        .HasColumnType("bigint")
                        .HasColumnName("id");

                    b.Property<byte[]>("Address")
                        .IsRequired()
                        .HasColumnType("bytea")
                        .HasColumnName("address");

                    b.PrimitiveCollection<int[]>("Ports")
                        .IsRequired()
                        .HasColumnType("integer[]")
                        .HasColumnName("ports");

                    b.Property<int>("ServerPort")
                        .HasColumnType("integer")
                        .HasColumnName("server_port");

                    b.Property<short>("Version")
                        .HasColumnType("smallint")
                        .HasColumnName("version");

                    b.HasKey("Id");

                    b.ToTable((string)null);

                    b.ToView("v_protocol_client", "traffic_graph_31");
                });

            modelBuilder.Entity("Ycode.Sniffy.Domain.ProtocolServerModel", b =>
                {
                    b.Property<long>("Id")
                        .HasColumnType("bigint")
                        .HasColumnName("id");

                    b.Property<byte[]>("Address")
                        .IsRequired()
                        .HasColumnType("bytea")
                        .HasColumnName("address");

                    b.Property<int>("Port")
                        .HasColumnType("integer")
                        .HasColumnName("port");

                    b.Property<short>("Version")
                        .HasColumnType("smallint")
                        .HasColumnName("version");

                    b.HasKey("Id");

                    b.ToTable((string)null);

                    b.ToView("v_protocol_server", "traffic_graph_31");
                });

            modelBuilder.Entity("Ycode.Sniffy.Domain.TrafficMetrics", b =>
                {
                    b.Property<byte[]>("DownAddress")
                        .IsRequired()
                        .HasColumnType("bytea")
                        .HasColumnName("down_address");

                    b.PrimitiveCollection<int[]>("DownPort")
                        .IsRequired()
                        .HasColumnType("integer[]")
                        .HasColumnName("down_ports");

                    b.Property<long>("DownSize")
                        .HasColumnType("bigint")
                        .HasColumnName("down_size");

                    b.Property<short>("DownVersion")
                        .HasColumnType("smallint")
                        .HasColumnName("down_version");

                    b.Property<DateTimeOffset>("Timestamp")
                        .HasColumnType("timestamp with time zone")
                        .HasColumnName("timestamp");

                    b.Property<byte[]>("UpAddress")
                        .IsRequired()
                        .HasColumnType("bytea")
                        .HasColumnName("up_address");

                    b.Property<int>("UpPort")
                        .HasColumnType("integer")
                        .HasColumnName("up_port");

                    b.Property<long>("UpSize")
                        .HasColumnType("bigint")
                        .HasColumnName("up_size");

                    b.Property<short>("UpVersion")
                        .HasColumnType("smallint")
                        .HasColumnName("up_version");

                    b.ToTable("traffic_metrics");
                });

            modelBuilder.Entity("Ycode.Sniffy.Domain.TrafficModel", b =>
                {
                    b.Property<long>("Id")
                        .HasColumnType("bigint")
                        .HasColumnName("id");

                    b.Property<long>("DownSize")
                        .HasColumnType("bigint")
                        .HasColumnName("down_size");

                    b.Property<long>("EndId")
                        .HasColumnType("bigint")
                        .HasColumnName("end");

                    b.Property<long>("StartId")
                        .HasColumnType("bigint")
                        .HasColumnName("start");

                    b.Property<long>("UpSize")
                        .HasColumnType("bigint")
                        .HasColumnName("up_size");

                    b.HasKey("Id");

                    b.HasIndex("EndId");

                    b.HasIndex("StartId");

                    b.ToTable((string)null);

                    b.ToView("v_traffic", "traffic_graph_31");
                });

            modelBuilder.Entity("Ycode.Sniffy.Domain.EndpointProtocolClientModel", b =>
                {
                    b.HasOne("Ycode.Sniffy.Domain.ProtocolClientModel", "End")
                        .WithMany()
                        .HasForeignKey("EndId")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();

                    b.HasOne("Ycode.Sniffy.Domain.EndpointModel", "Start")
                        .WithMany()
                        .HasForeignKey("StartId")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();

                    b.Navigation("End");

                    b.Navigation("Start");
                });

            modelBuilder.Entity("Ycode.Sniffy.Domain.EndpointProtocolServerModel", b =>
                {
                    b.HasOne("Ycode.Sniffy.Domain.ProtocolServerModel", "End")
                        .WithMany()
                        .HasForeignKey("EndId")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();

                    b.HasOne("Ycode.Sniffy.Domain.EndpointModel", "Start")
                        .WithMany()
                        .HasForeignKey("StartId")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();

                    b.Navigation("End");

                    b.Navigation("Start");
                });

            modelBuilder.Entity("Ycode.Sniffy.Domain.TrafficModel", b =>
                {
                    b.HasOne("Ycode.Sniffy.Domain.ProtocolServerModel", "End")
                        .WithMany()
                        .HasForeignKey("EndId")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();

                    b.HasOne("Ycode.Sniffy.Domain.ProtocolClientModel", "Start")
                        .WithMany()
                        .HasForeignKey("StartId")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();

                    b.Navigation("End");

                    b.Navigation("Start");
                });
#pragma warning restore 612, 618
        }
    }
}
