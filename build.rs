extern crate cmake;
use cmake::Config;


fn main() {
    let dst = Config::new("src/cpp").build();

    println!("cargo:rustc-link-search=native={}", dst.display());
    println!("cargo:rustc-link-lib=static=dng");
    println!("cargo:rustc-link-lib=static=dng_sdk");
    println!("cargo:rustc-link-lib=static=xmp-sdk");
    println!("cargo:rustc-link-lib=dylib=stdc++");
    println!("cargo:rustc-link-lib=dylib=jpeg");
    println!("cargo:rustc-link-lib=dylib=expat");
    println!("cargo:rustc-link-lib=dylib=z");
}
