extern crate cmake;
extern crate reqwest;

use cmake::Config;
use std::env;
use std::fs::File;
use std::io;

fn download_file(url: &str, out_filepath: &str) {
    let mut resp = reqwest::blocking::get(url).unwrap();
    let mut output_file = File::create(out_filepath).unwrap();
    io::copy(&mut resp, &mut output_file).unwrap();
}

fn main() {
    let target = env::var("TARGET").unwrap();

    let dst = if target.contains("apple") || target.contains("linux") {
        Config::new("src/cpp").build()
    } else {
        Config::new("src/cpp")
            .static_crt(false)
            .define("VCPKG_TARGET_TRIPLET", "x64-windows-static")
            .build()
    };

    let path = dst.display();

    println!("cargo:rustc-link-search=native={}/lib", path);
    println!("cargo:rustc-link-lib=static=dngbindings");
    println!("cargo:rustc-link-lib=static=dng-sdk");
    println!("cargo:rustc-link-lib=static=xmp-sdk");

    if target.contains("apple") {
        println!("cargo:rustc-link-lib=framework=CoreFoundation");
        println!("cargo:rustc-link-lib=framework=CoreServices");
        println!("cargo:rustc-link-lib=dylib=c++");
        println!(
            "cargo:rustc-link-search=native={}/build/vcpkg_installed/x64-osx/lib",
            path
        );
        println!("cargo:rustc-link-lib=static=expat");
        println!("cargo:rustc-link-lib=static=jpeg");
        println!("cargo:rustc-link-lib=static=z");
    } else if target.contains("linux") {
        println!("cargo:rustc-link-lib=dylib=stdc++");
        println!(
            "cargo:rustc-link-search=native={}/build/vcpkg_installed/x64-linux/lib",
            path
        );
        println!("cargo:rustc-link-lib=static=expat");
        println!("cargo:rustc-link-lib=static=jpeg");
        println!("cargo:rustc-link-lib=static=z");
    } else {
        download_file(
            "https://github.com/microsoft/vcpkg/files/7075269/nasm-2.15.05-win32.zip",
            "src/cpp/vcpkg/downloads/nasm-2.15.05-win32.zip",
        );

        println!(
            "cargo:rustc-link-search=native={}/build/vcpkg_installed/x64-windows-static/lib",
            path
        );
        println!("cargo:rustc-link-lib=static=libexpatMD");
        println!("cargo:rustc-link-lib=static=jpeg");
        println!("cargo:rustc-link-lib=static=zlib");
    }
}
