extern "C" {
    pub fn dummy();
}

pub fn call_dummy() {
    unsafe { dummy(); }
}

#[cfg(test)]
mod tests {
    extern {
        #[link(name = "dng", kind = "static")]
        fn dummy();
    }

    #[test]
    fn it_works() {
        unsafe {
            dummy();
        }
    }
}
