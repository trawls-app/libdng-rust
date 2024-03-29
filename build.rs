extern crate cmake;
use cmake::Config;
use std::env;


fn main() {
    let target  = env::var("TARGET").unwrap();

    let (dst, triplet) = if target.contains("apple") && (target.contains("aarch64") || target.contains("arm64")) {
        (Config::new("src/cpp").define("VCPKG_TARGET_TRIPLET", "arm64-osx").build(), "arm64-osx")
    } else if target.contains("apple") {
        (Config::new("src/cpp").build(), "x64-osx")
    } else if target.contains("linux") {
        (Config::new("src/cpp").build(), "x64-linux")
    } else {
        (Config::new("src/cpp").static_crt(false).define("VCPKG_TARGET_TRIPLET", "x64-windows-static").build(), "x64-windows-static")
    };

    let path = dst.display();

    println!("cargo:rustc-link-search=native={}/lib", path);
    println!("cargo:rustc-link-search=native={}/build/vcpkg_installed/{}/lib", path, triplet);
    println!("cargo:rustc-link-lib=static=dngbindings");
    println!("cargo:rustc-link-lib=static=dng-sdk");
    println!("cargo:rustc-link-lib=static=xmp-sdk");

    if target.contains("apple")
    {
        println!("cargo:rustc-link-lib=framework=CoreFoundation");
        println!("cargo:rustc-link-lib=framework=CoreServices");
        println!("cargo:rustc-link-lib=dylib=c++");
        println!("cargo:rustc-link-lib=static=expat");
        println!("cargo:rustc-link-lib=static=jpeg");
        println!("cargo:rustc-link-lib=static=z");
    }
    else if target.contains("linux")
    {
        println!("cargo:rustc-link-lib=dylib=stdc++");
        println!("cargo:rustc-link-lib=static=expat");
        println!("cargo:rustc-link-lib=static=jpeg");
        println!("cargo:rustc-link-lib=static=z");
    }
    else
    {
        println!("cargo:rustc-link-lib=static=libexpatMD");
        println!("cargo:rustc-link-lib=static=jpeg");
        println!("cargo:rustc-link-lib=static=zlib");
    }
}
