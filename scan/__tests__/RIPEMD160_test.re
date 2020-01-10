open Jest;
open RIPEMD160;
open Expect;

describe("Expect RIPEMD160 to work correctly", () => {
  test("should be able to hash int array in to int array format", () =>
    expect(
      [|
        17,
        17,
        51,
        217,
        158,
        48,
        5,
        97,
        37,
        254,
        228,
        94,
        78,
        113,
        64,
        167,
        138,
        8,
        230,
        166,
        33,
        234,
        187,
        134,
        187,
        29,
        252,
        165,
        103,
        160,
        138,
        186,
      |]
      ->digest,
    )
    |> toEqual([|
         136,
         183,
         105,
         178,
         192,
         84,
         36,
         85,
         62,
         1,
         17,
         94,
         138,
         140,
         162,
         151,
         102,
         116,
         80,
         245,
       |])
  );
  test("should be able to hash int array in to int array format", () =>
    expect(
      [|
        17,
        17,
        51,
        217,
        158,
        48,
        5,
        97,
        37,
        254,
        228,
        94,
        78,
        113,
        64,
        167,
        138,
        8,
        230,
        166,
        33,
        234,
        187,
        134,
        187,
        29,
        252,
        165,
        103,
        160,
        138,
        186,
      |]
      ->hexDigest,
    )
    |> toBe("88b769b2c05424553e01115e8a8ca297667450f5")
  );
});