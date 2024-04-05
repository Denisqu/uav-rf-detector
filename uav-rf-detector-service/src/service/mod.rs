//use tokio::sync::mpsc::{Receiver, Sender};
use crossbeam_channel;
use tonic::{Request, Response, Status};
use tonic::codegen::tokio_stream::wrappers::ReceiverStream;
use crate::proto::{Detection, detector_service_server, Ping};
use tokio::sync::mpsc;
use tonic::codegen::Body;

pub struct DetectorService<T> {
    ml_rx: crossbeam_channel::Receiver<T>,
    ml_tx: crossbeam_channel::Sender<T>
}

impl<T> DetectorService<T> {
    fn new() -> DetectorService<T> {
        //let (tx, rx) = mpsc::channel(1024);
        let (tx, rx) = crossbeam_channel::unbounded();
        DetectorService {
            ml_rx: rx,
            ml_tx: tx
        }
    }

    fn get_sender(&self) -> crossbeam_channel::Sender<T> {
        return self.ml_tx.clone();
    }
}

#[tonic::async_trait]
impl<T: std::marker::Send> detector_service_server::DetectorService for DetectorService<T> {
    type GetDetectionsStreamStream = ReceiverStream<Result<Detection, Status>>;

    async fn get_detections_stream(&self, request: Request<()>) -> Result<Response<Self::GetDetectionsStreamStream>, Status> {
        let (mut tx, rx) = mpsc::channel(120);

        tokio::spawn(async move {
            loop {
                tx.send(self.ml_rx.recv().await).await.unwrap();
            }
        });

        /*tokio::spawn(async move {
            for i in 0..10 {
                tx.send(Ok(Detection{value: "test".to_string(), timestamp: i.to_string()})).await.unwrap();
            }
        });*/

        return Ok(Response::new(ReceiverStream::new(rx)))
    }

    async fn get_ping(&self, request: Request<Ping>) -> Result<Response<Ping>, Status> {
        return Ok(Response::new(Ping{value: request.into_inner().value}))
    }
}
