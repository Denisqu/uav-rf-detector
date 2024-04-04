use tonic::{Request, Response, Status};
use tonic::codegen::tokio_stream::wrappers::ReceiverStream;
use crate::proto::{Detection, detector_service_server};
use tokio::sync::mpsc;
use tonic::codegen::Body;

pub struct DetectorService {}

#[tonic::async_trait]
impl detector_service_server::DetectorService for DetectorService {
    type startDetectionsStreamStream = ReceiverStream<Result<Detection, Status>>;

    async fn start_detections_stream(&self, request: Request<()>) -> Result<Response<Self::startDetectionsStreamStream>, Status> {
        let (mut tx, rx) = mpsc::channel(120);

        tokio::spawn(async move {
            for i in 0..10 {
                tx.send(Ok(Detection::default())).await.unwrap();
            }
        });

        return Ok(Response::new(ReceiverStream::new(rx)))
    }
}