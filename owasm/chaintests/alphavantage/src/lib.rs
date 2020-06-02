use obi::{OBIDecode, OBIEncode};
use owasm::{execute_entry_point, ext, oei, prepare_entry_point};

#[derive(OBIDecode)]
struct Input {
    symbol: String,
    api_key: String,
    multiplier: u64,
}

#[derive(OBIEncode)]
struct Output {
    px: u64,
}

#[no_mangle]
fn prepare_impl(input: Input) {
    // Alphavantage price data source
    oei::request_external_data(6, 1, format!("{} {}", input.symbol, input.api_key).as_bytes());
}

#[no_mangle]
fn execute_impl(input: Input) -> Output {
    let avg: f64 = ext::load_average(1);
    Output { px: (avg * input.multiplier as f64) as u64 }
}

prepare_entry_point!(prepare_impl);
execute_entry_point!(execute_impl);
