use std::env;
use rawloader;
use std::path::Path;

use libdng::exif::ExifBox;
use libdng::image_info::DNGWriting;


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
    let writer = image.get_dng_writer().unwrap();

    println!("Make: '{}',\tModel: '{}'", image.clean_make, image.clean_model);
    println!("Width: {},\tHeight: {}", image.width, image.height);

    /*println!("\nCam to XYZ");
    for row in image.cam_to_xyz().iter() {
        for v in row {
            print!(" {}", v);
        }
        println!();
    }

    println!("\nCam to XYZ (normalized)");
    for row in image.cam_to_xyz_normalized().iter() {
        for v in row {
            print!(" {}", v);
        }
        println!();
    }*/

    println!("\nXYZ to cam");
    for row in image.xyz_to_cam.iter() {
        for v in row {
            print!(" {}", v);
        }
        println!();
    }

    print!("\nBlack levels: ");
    for bl in image.blacklevels.iter() {
        print!(" {}", bl);
    }

    print!("\nWhite levels: ");
    for wl in image.whitelevels.iter() {
        print!(" {}", wl);
    }

    print!("\nWB Coeffs (cam_mul): ");
    for wbc in image.wb_coeffs.iter() {
        print!(" {}", wbc);
    }

    print!("\nCrops (top, right, bottom, left): ");
    for cr in image.crops.iter() {
        print!(" {}", cr);
    }

    println!("\n\nBlack areas");
    for ba in image.blackareas.iter() {
        println!(" {} {} {} {}", ba.0, ba.1, ba.2, ba.3);
    }

    println!("\nCFA: {}\n", image.cfa.name);

    writer.write_jpg(Path::new(file_out_jpg.as_str()));
    writer.write_dng(Path::new(file_out_dng.as_str()))
}