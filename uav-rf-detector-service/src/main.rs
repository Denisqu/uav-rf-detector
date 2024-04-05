use std::net::ToSocketAddrs;
use tokio::sync::mpsc;
use std::thread;
use std::time::Duration;
use tonic::transport::Server;
use proto::detector_service_server;
use chrono;
use crate::proto::Detection;

mod proto;
mod service;

// TODO: https://github.com/hyperium/tonic/blob/master/examples/routeguide-tutorial.md

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    tonic_build::compile_protos("../uav-rf-detector-client/files/proto/rf_detector.proto")?;

    // ML-executor thread
    let (ml_tx, mut ml_rx) = mpsc::channel(1024);
    let ml_thread_handle = thread::spawn(move || {
        loop {
            println!("sending result from ML-executor thread");
            let result = ml_tx.blocking_send(Detection {
                value: "detection-name".to_string(), timestamp: chrono::prelude::Utc::now().to_string()
            });

            match result {
                Ok(_) => (),
                Err(err) => panic!("{}", err)
            }

            thread::sleep(Duration::new(1, 0));
        }
    });

    // ML-Listener task
    let detector_service = service::DetectorService::new();
    let tx = detector_service.get_sender();
    let ml_listener_handle = tokio::spawn(async move {
        let tx = tx;
        loop {
            println!("sending result from ML-listener task");
            let result = ml_rx.recv().await;
            tx.try_broadcast(result.unwrap()).unwrap();
       }
    });

    // TODO: add sqlx postgress connection
    // ...

    Server::builder()
        .add_service(detector_service_server::DetectorServiceServer::new(detector_service))
        .serve("127.0.0.1:50051".to_socket_addrs().unwrap().next().unwrap())
        .await
        .unwrap();

    ml_thread_handle.join().unwrap();
    ml_listener_handle.await.unwrap();

    return Ok(());
}
