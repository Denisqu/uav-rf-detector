

fn main() -> Result<(), Box<dyn std::error::Error>> {
    tonic_build::compile_protos("../uav-rf-detector-client/files/proto/rf_detector.proto")?;


    Ok(())
}
