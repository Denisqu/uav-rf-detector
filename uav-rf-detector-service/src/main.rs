use std::net::ToSocketAddrs;
use tonic::transport::Server;
use proto::detector_service_server;

mod proto;
mod service;

// TODO: https://github.com/hyperium/tonic/blob/master/examples/routeguide-tutorial.md

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    tonic_build::compile_protos("../uav-rf-detector-client/files/proto/rf_detector.proto")?;

    let detector_service = service::DetectorService {};

    Server::builder()
        .add_service(detector_service_server::DetectorServiceServer::new(detector_service))
        .serve("127.0.0.1:50051".to_socket_addrs().unwrap().next().unwrap())
        .await
        .unwrap();

    return Ok(());
}
