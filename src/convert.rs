use std::env;
use rawloader;
use std::convert::TryInto;
use std::path::Path;

use ::libdng::DNGWriter;


fn main() {
    let args: Vec<_> = env::args().collect();
    if args.len() != 3 {
        println!("Usage: {} <file>", args[0]);
        std::process::exit(2);
    }

    let file_in = &args[1];
    let file_out = &args[2];
    let file_out_jpg = file_out.to_owned() + ".jpg";
    let file_out_dng = file_out.to_owned() + ".dng";

    println!("Converting '{}' to '{}' and '{}'", file_in, file_out_jpg, file_out_dng);
    let image = rawloader::decode_file(file_in).unwrap();

    let writer = DNGWriter::new(
        image.width.try_into().unwrap(),
        image.height.try_into().unwrap()
    );

    if let rawloader::RawImageData::Integer(data) = image.data {
        writer.build_negative(data);
    } else {
        unimplemented!("Can't parse RAWs with non-integer data, yet.");
    }

    writer.write_jpg(Path::new(file_out_jpg.as_str()));
    writer.write_dng(Path::new(file_out_dng.as_str()))
}