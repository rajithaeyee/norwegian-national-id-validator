open Jest;
open ExpectJs;
open Index;

describe("isValidIdNumberFormat", () => {
  test("eleven digits is valid", () =>
    expect(isValidIdNumberFormat("12345678901")) |> toBeTruthy
  );
  test("eleven digits with whitespace is valid", () =>
    expect(isValidIdNumberFormat("   12345678901  ")) |> toBeTruthy
  );
  test("ten digits is invalid", () =>
    expect(isValidIdNumberFormat("1234567890")) |> toBeFalsy
  );
  test("ten digits with whitespace is invalid", () =>
    expect(isValidIdNumberFormat("1234567890 ")) |> toBeFalsy
  );
  test("eleven characters with some non-digits is invalid", () =>
    expect(isValidIdNumberFormat("12345h67890")) |> toBeFalsy
  );
});

describe("isValidCheckDigits", () => {
  test("valid check digits", () =>
    expect(isValidCheckDigits("01010107543")) |> toBeTruthy
  );
  test("invalid check digits", () =>
    expect(isValidCheckDigits("01010107549")) |> toBeFalsy
  );
});

describe("getIdNumberType returns correct type", () => {
  test("D-number 1", () =>
    expect(getIdNumberType("42059199212")) |> toEqual(Some(DNumber))
  );
  test("D-number 2", () =>
    expect(getIdNumberType("67047000642")) |> toEqual(Some(DNumber))
  );
  test("FH-number 1", () =>
    expect(getIdNumberType("81234567802")) |> toEqual(Some(FHNumber))
  );
  test("FH-number 2", () =>
    expect(getIdNumberType("91234567883")) |> toEqual(Some(FHNumber))
  );
  test("H-number 1", () =>
    expect(getIdNumberType("01415612385")) |> toEqual(Some(HNumber))
  );
  test("H-number 2", () =>
    expect(getIdNumberType("01535612303")) |> toEqual(Some(HNumber))
  );
  test("birth number", () =>
    expect(getIdNumberType("01010102576")) |> toEqual(Some(BirthNumber))
  );
});

describe("getBirthCenturyFromIdNumber", () => {
  /* ID numbers does not have valid check digits, otherwise valid */
  test("18th century for normal birthnumber", () =>
    expect(getBirthCenturyFromIdNumber("01015450131")) |> toBe("18")
  );
  test("19th century for normal birthnumber (low individual number)", () =>
    expect(getBirthCenturyFromIdNumber("01010100131")) |> toBe("19")
  );
  test("19th century for normal birthnumber (high individual number)", () =>
    expect(getBirthCenturyFromIdNumber("01045499931")) |> toBe("19")
  );
  test("20th century for normal birthnumber (low individual number)", () =>
    expect(getBirthCenturyFromIdNumber("01010150131")) |> toBe("20")
  );
  test("20th century for normal birthnumber (high individual number)", () =>
    expect(getBirthCenturyFromIdNumber("01010199931")) |> toBe("20")
  );
  test("19th century for D-number (high individual number)", () =>
    expect(getBirthCenturyFromIdNumber("42059199212")) |> toBe("19")
  );
});

describe("getBirthDate - functions", () => {
  test("birth date is found for D-number", () =>
    expect(possibleBirthDateOfDNumber("67047000642")) |> toBe("27041970")
  );

  test("birth date is found for birth number", () =>
    expect(possibleBirthDateOfBirthNumber("01010139461"))
    |> toBe("01011901")
  );

  test("birth date is found for H-number", ()
    /* TODO: This test has is invalid check digits */
    =>
      expect(possibleBirthDateOfHNumber("01410100131")) |> toBe("01011901")
    );
});

describe("validateNorwegianIdNumber", () => {
  test("knows that no one could possibly be born on 29. Feb 1999", () =>
    expect(validateNorwegianIdNumber("29029900157")) |> toBe(false)
  );

  test("knows that it is possible to be born on 29. Feb 1996", () =>
    expect(validateNorwegianIdNumber("29029600013")) |> toBe(true)
  );

  test("does not accept future valid ID numbers", () =>
    expect(validateNorwegianIdNumber("24088951559")) |> toBe(false)
  );
});

describe("does not accept ID numbers with invalid check digits", () => {
  test("invalid check digit 1", () =>
    expect(validateNorwegianIdNumber("81234567803")) |> toBe(false)
  );
  test("invalid check digit 2", () =>
    expect(validateNorwegianIdNumber("01415612381")) |> toBe(false)
  );
  test("invalid check digit 3", () =>
    expect(validateNorwegianIdNumber("03119975255")) |> toBe(false)
  );
  test("invalid check digit 4", () =>
    expect(validateNorwegianIdNumber("67047000658")) |> toBe(false)
  );
});

[%bs.raw
  {|
describe("validateNorwegianIdNumber rawJS", () => {
  var validMaleNumbers = require("./testdata/listOfPersonalIdNumbers.bs").validMaleIdNumbers;
  var validFemaleNumbers = require("./testdata/listOfPersonalIdNumbers.bs").validFemaleIdNumbers;
  var validateNorwegianIdNumber = require("../src/index.bs").validateNorwegianIdNumber;

  it("works for valid birth numbers for men born on 1. Jan 1901", () => {
    for (var number of validMaleNumbers) {
      expect(validateNorwegianIdNumber(number)).toBeTruthy()
    }
  })

  it("works for valid birth numbers for women born on 1. Jan 1901", () => {
    for (var number of validFemaleNumbers) {
      expect(validateNorwegianIdNumber(number)).toBeTruthy()
    }
  })
})
|}
];

describe("A Norwegian person number (last 5 digits of ID number)", () => {
  open BsJestDateMock;

  afterEach(() => clear());
  beforeEach(() =>
    advanceTo(
      Js.Date.makeWithYMDH(
        ~year=2017.0,
        ~month=6.0,
        ~date=18.0,
        ~hours=12.0,
        (),
      ),
    )
  );

  test(
    "belongs to a person born in the 1900s if the three first digits are in the [0, 500) range",
    () =>
    expect(possibleAgeOfPersonWithIdNumber("03119849925"))
    |> toBe(Some(18))
  );

  test(
    "belongs to a person born in the 1800s or 2000s if the three first digits are in the [500, 750) range #1",
    () =>
    expect(possibleAgeOfPersonWithIdNumber("04119850938"))
    |> toBe(Some(118))
  );

  test(
    "belongs to a person born in the 1800s or 2000s if the three first digits are in the [500, 750) range #2",
    () =>
    expect(possibleAgeOfPersonWithIdNumber("04110250989"))
    |> toBe(Some(14))
  );

  test(
    "belongs to a person born in the 1900s or 2000s if the three first digits are in the [900, 1000) range #1",
    () =>
    expect(possibleAgeOfPersonWithIdNumber("03111590981")) |> toBe(Some(1))
  );
  test(
    "belongs to a person born in the 1900s or 2000s if the three first digits are in the [900, 1000) range #2",
    () =>
    expect(possibleAgeOfPersonWithIdNumber("03115690905"))
    |> toBe(Some(60))
  );

  test(
    "belongs to a person born in the 2000s if the three first digits are in the [750, 900) range",
    () =>
    expect(possibleAgeOfPersonWithIdNumber("03110175255"))
    |> toBe(Some(15))
  );
  test("returns None when asking for age of a future date", () =>
    expect(possibleAgeOfPersonWithIdNumber("03119975246")) |> toBe(None)
  );

  test("is not part of an FH number", () =>
    expect(possibleAgeOfPersonWithIdNumber("83119849925")) |> toBe(None)
  );

  test(
    "cannot be meaningfully extracted from an ID number shorter than 11 digits",
    () =>
    expect(possibleAgeOfPersonWithIdNumber("1111111111")) |> toBe(None)
  );
});