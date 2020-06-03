use obi::{OBIDecode, OBIEncode};
use owasm::{execute_entry_point, ext, oei};

#[derive(OBIDecode)]
struct Input {
    multiplier: u64,
}

#[derive(OBIEncode)]
struct Output {
    px: u64,
}

#[no_mangle]
fn prepare() {
    // Gold price data source
    oei::request_external_data(5, 1, "".as_bytes());
    // Binance data source
    oei::request_external_data(3, 2, "ATOM".as_bytes());
}

#[no_mangle]
fn execute_impl(input: Input) -> Output {
    let avg_gold: f64 = ext::load_average(1);
    let avg_atom: f64 = ext::load_average(2);
    Output { px: (avg_gold * input.multiplier as f64 / avg_atom) as u64 }
}

execute_entry_point!(execute_impl);
