
pub mod proto;
//use rf_detector::detector_service_server::{DetectorServiceServer};

// TODO: https://github.com/hyperium/tonic/blob/master/examples/routeguide-tutorial.md
fn main() -> Result<(), Box<dyn std::error::Error>> {
    tonic_build::compile_protos("../uav-rf-detector-client/files/proto/rf_detector.proto")?;
    //let s = DetectorServiceServer::new(12);

    Ok(())
}
