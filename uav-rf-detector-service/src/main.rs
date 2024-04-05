use std::net::ToSocketAddrs;
//use std::sync::mpsc;
use tokio::sync::mpsc;
use std::thread;
use std::time::Duration;
use tonic::transport::Server;
use proto::detector_service_server;
use std::time::Instant;
use chrono;
//use tonic::transport::{Certificate, Channel, ClientTlsConfig};

mod proto;
mod service;

// TODO: https://github.com/hyperium/tonic/blob/master/examples/routeguide-tutorial.md

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    tonic_build::compile_protos("../uav-rf-detector-client/files/proto/rf_detector.proto")?;

    let (ml_tx, mut ml_rx) = mpsc::channel(1024);

    // ML-executor thread
    let ml_thread_handle = thread::spawn(move || {
        let tx = ml_tx;
        loop {
            println!("sending current time from ml_thread...");
            tx.try_send(chrono::prelude::Utc::now().to_string()).unwrap();
            thread::sleep(Duration::new(1, 0));
        }
    });

    // ML-Listener task
    let ml_listener_handle = tokio::spawn(async move {
       loop {
            let result = ml_rx.recv().await;
            println!("{}", result.unwrap_or("no result".to_string()));
       }
    });

    // TODO: add sqlx postgress connection
    // ...

    let detector_service = service::DetectorService {};
    Server::builder()
        .add_service(detector_service_server::DetectorServiceServer::new(detector_service))
        .serve("127.0.0.1:50051".to_socket_addrs().unwrap().next().unwrap())
        .await
        .unwrap();

    ml_thread_handle.join().unwrap();
    ml_listener_handle.await.unwrap();

    return Ok(());
}
