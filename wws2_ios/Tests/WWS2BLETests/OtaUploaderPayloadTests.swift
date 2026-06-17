import XCTest
@testable import WWS2BLE

final class OtaUploaderPayloadTests: XCTestCase {

    func testPreparePayloadRejectsApplicationOnlyFirmware() {
        let app = appImage()

        switch OtaUploader.preparePayloadForUpload(app) {
        case .valid:
            XCTFail("Application-only BIN must be rejected; OTA requires combined bootloader+application BIN")
        case .invalid(message: let message):
            XCTAssertTrue(message.contains("Application-only"))
        }
    }

    func testPreparePayloadTrimsCombinedBootloaderAndAppImage() {
        let app = appImage()
        var combined = bootloaderImage()
        combined += Array(repeating: 0xFF, count: OtaUploader.bootloaderTrimOffset - combined.count)
        combined += app

        switch OtaUploader.preparePayloadForUpload(combined) {
        case .valid(payload: let payload, kind: let kind):
            XCTAssertEqual(kind, .combinedBootloaderAndApp)
            XCTAssertEqual(payload, app)
        case .invalid(message: let message):
            XCTFail("Expected valid combined image, got: \(message)")
        }
    }

    func testPreparePayloadRejectsPaddedApplicationImageWithoutBootloader() {
        let app = appImage()
        let padded = Array(repeating: UInt8(0xFF), count: OtaUploader.bootloaderTrimOffset) + app

        switch OtaUploader.preparePayloadForUpload(padded) {
        case .valid:
            XCTFail("Padded application without bootloader must be rejected")
        case .invalid(message: let message):
            XCTAssertTrue(message.contains("bootloader vector table"))
        }
    }

    func testPreparePayloadRejectsBank2LinkedImage() {
        let bank2Linked = vector(reset: 0x0810_0101) + [0xAA, 0x55]

        switch OtaUploader.preparePayloadForUpload(bank2Linked) {
        case .valid:
            XCTFail("Bank2-linked image must not be accepted for copy-to-app OTA")
        case .invalid(message: let message):
            XCTAssertTrue(message.contains("0x08100000") || message.contains("combined"))
        }
    }

    func testPreparePayloadRejectsRandomData() {
        let random = Array(0..<64).map(UInt8.init)

        switch OtaUploader.preparePayloadForUpload(random) {
        case .valid:
            XCTFail("Random data must not be accepted as OTA firmware")
        case .invalid(message: let message):
            XCTAssertTrue(message.contains("combined") || message.contains("bootloader"))
        }
    }

    private func appImage() -> [UInt8] {
        vector(reset: 0x0800_8101) + [0xAA, 0x55, 0x11, 0x22]
    }

    private func bootloaderImage() -> [UInt8] {
        vector(reset: 0x0800_0101) + [0xBB, 0x66]
    }

    private func vector(reset: UInt32) -> [UInt8] {
        le32(0x2008_0000) + le32(reset)
    }

    private func le32(_ value: UInt32) -> [UInt8] {
        [
            UInt8(value & 0xFF),
            UInt8((value >> 8) & 0xFF),
            UInt8((value >> 16) & 0xFF),
            UInt8((value >> 24) & 0xFF),
        ]
    }
}
