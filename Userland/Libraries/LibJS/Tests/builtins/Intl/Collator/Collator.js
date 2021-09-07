describe("errors", () => {
    test("called without new", () => {
        expect(() => {
            Intl.Collator();
        }).toThrowWithMessage(TypeError, "Intl.Collator constructor must be called with 'new'");
    });
});

describe("normal behavior", () => {
    test("length is 0", () => {
        expect(Intl.Collator).toHaveLength(0);
    });
});
