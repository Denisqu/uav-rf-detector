use tonic::{Request, Response, Status};
use tonic::codegen::tokio_stream::wrappers::ReceiverStream;
use crate::proto::{Detection, detector_service_server, Ping};
use tokio::sync::mpsc;
use async_broadcast;
use async_broadcast::broadcast;

pub struct DetectorService {
    ml_rx: async_broadcast::Receiver<Detection>,
    ml_tx: async_broadcast::Sender<Detection>
}

impl DetectorService {
    pub fn new() -> DetectorService {
        let (tx, mut rx) = broadcast(1);
        rx.set_overflow(true);
        DetectorService {
            ml_rx: rx,
            ml_tx: tx
        }
    }

    pub fn get_sender(&self) -> async_broadcast::Sender<Detection> {
        return self.ml_tx.clone();
    }
}

#[tonic::async_trait]
impl detector_service_server::DetectorService for DetectorService {
    type GetDetectionsStreamStream = ReceiverStream<Result<Detection, Status>>;

    async fn get_detections_stream(&self, _request: Request<()>) -> Result<Response<Self::GetDetectionsStreamStream>, Status> {
        let (tx, rx) = mpsc::channel(1);

        let mut ml_rx_clone = self.ml_rx.clone();

        tokio::spawn(async move {
            loop {
                let result = ml_rx_clone.recv().await;
                match result {
                    Ok(result) => match tx.send(Ok(result)).await {
                        Ok(_) => { continue },
                        Err(error) => { println!("{}", error); break; }
                    }
                    Err(error) => println!("{}", error.to_string())
                }
            }
        });

        return Ok(Response::new(ReceiverStream::new(rx)))
    }

    async fn get_ping(&self, request: Request<Ping>) -> Result<Response<Ping>, Status> {
        return Ok(Response::new(Ping{value: request.into_inner().value}))
    }
}
