#include "machineconfig.h"

#include <QtTest>

using namespace machine;

class TestMachineConfig : public QObject {
    Q_OBJECT

private slots:
    void default_round_trip();
    void complete_round_trip();
    void rejects_invalid_json();
};

void TestMachineConfig::default_round_trip() {
    MachineConfig original;
    QString error;
    auto decoded = MachineConfig::from_json(original.to_json(), &error);

    QVERIFY2(decoded.has_value(), qPrintable(error));
    QVERIFY(decoded.value() == original);
}

void TestMachineConfig::complete_round_trip() {
    MachineConfig original;
    original.set_simulated_endian(BIG);
    original.set_simulated_xlen(Xlen::_64);
    original.set_isa_word(
        config_isa_word_default | ConfigIsaWord::byChar('F') | ConfigIsaWord::byChar('D'));
    original.set_pipelined(true);
    original.set_delay_slot(false);
    original.set_hazard_unit(MachineConfig::HU_STALL);
    original.set_memory_execute_protection(true);
    original.set_memory_write_protection(true);
    original.set_memory_access_time_read(17);
    original.set_memory_access_time_write(19);
    original.set_memory_access_time_burst(3);
    original.set_memory_access_time_level2(5);
    original.set_memory_access_enable_burst(true);
    original.set_osemu_enable(false);
    original.set_osemu_known_syscall_stop(false);
    original.set_osemu_unknown_syscall_stop(false);
    original.set_osemu_interrupt_stop(false);
    original.set_osemu_exception_stop(false);
    original.set_osemu_fs_root("/benchmark-root");
    original.set_reset_at_compile(false);
    original.set_elf("/tmp/workload.elf");

    CacheConfig program_cache;
    program_cache.set_enabled(true);
    program_cache.set_set_count(8);
    program_cache.set_block_size(4);
    program_cache.set_associativity(2);
    program_cache.set_replacement_policy(CacheConfig::RP_PLRU);
    program_cache.set_write_policy(CacheConfig::WP_THROUGH_NOALLOC);
    original.set_cache_program(program_cache);

    CacheConfig data_cache;
    data_cache.set_enabled(true);
    data_cache.set_set_count(16);
    data_cache.set_block_size(8);
    data_cache.set_associativity(4);
    data_cache.set_replacement_policy(CacheConfig::RP_NMRU);
    data_cache.set_write_policy(CacheConfig::WP_BACK);
    original.set_cache_data(data_cache);

    CacheConfig level2_cache;
    level2_cache.set_enabled(true);
    level2_cache.set_set_count(32);
    level2_cache.set_block_size(16);
    level2_cache.set_associativity(8);
    level2_cache.set_replacement_policy(CacheConfig::RP_LFU);
    level2_cache.set_write_policy(CacheConfig::WP_THROUGH_ALLOC);
    original.set_cache_level2(level2_cache);

    original.set_bp_enabled(true);
    original.set_bp_type(PredictorType::SMITH_2_BIT_HYSTERESIS);
    original.set_bp_init_state(PredictorState::WEAKLY_TAKEN);
    original.set_bp_btb_bits(5);
    original.set_bp_bhr_bits(6);
    original.set_bp_bht_addr_bits(7);

    original.set_vm_enabled(true);
    original.access_tlb_program()->set_vm_asid(11);
    original.access_tlb_program()->set_tlb_num_sets(8);
    original.access_tlb_program()->set_tlb_associativity(2);
    original.access_tlb_program()->set_tlb_replacement_policy(TLBConfig::RP_PLRU);
    original.access_tlb_data()->set_vm_asid(13);
    original.access_tlb_data()->set_tlb_num_sets(16);
    original.access_tlb_data()->set_tlb_associativity(4);
    original.access_tlb_data()->set_tlb_replacement_policy(TLBConfig::RP_LFU);

    QString error;
    auto decoded = MachineConfig::from_json(original.to_json(), &error);

    QVERIFY2(decoded.has_value(), qPrintable(error));
    QVERIFY(decoded.value() == original);
}

void TestMachineConfig::rejects_invalid_json() {
    MachineConfig config;

    QJsonObject future_version = config.to_json();
    future_version["version"] = 2;
    QString error;
    QVERIFY(!MachineConfig::from_json(future_version, &error).has_value());
    QVERIFY(error.contains("version"));

    QJsonObject invalid_policy = config.to_json();
    QJsonObject machine = invalid_policy["machine"].toObject();
    QJsonObject caches = machine["caches"].toObject();
    QJsonObject data_cache = caches["data"].toObject();
    data_cache["replacement_policy"] = "invalid";
    caches["data"] = data_cache;
    machine["caches"] = caches;
    invalid_policy["machine"] = machine;
    QVERIFY(!MachineConfig::from_json(invalid_policy, &error).has_value());
    QVERIFY(error.contains("replacement_policy"));

    QJsonObject missing_machine = config.to_json();
    missing_machine.remove("machine");
    QVERIFY(!MachineConfig::from_json(missing_machine, &error).has_value());
    QVERIFY(error.contains("machine"));

    QJsonObject invalid_predictor = config.to_json();
    machine = invalid_predictor["machine"].toObject();
    QJsonObject predictor = machine["branch_predictor"].toObject();
    predictor["type"] = "undefined";
    machine["branch_predictor"] = predictor;
    invalid_predictor["machine"] = machine;
    QVERIFY(!MachineConfig::from_json(invalid_predictor, &error).has_value());
    QVERIFY(error.contains("must not be"));

    QJsonObject invalid_tlb = config.to_json();
    machine = invalid_tlb["machine"].toObject();
    QJsonObject virtual_memory = machine["virtual_memory"].toObject();
    QJsonObject program_tlb = virtual_memory["program_tlb"].toObject();
    program_tlb["sets"] = 3;
    virtual_memory["program_tlb"] = program_tlb;
    machine["virtual_memory"] = virtual_memory;
    invalid_tlb["machine"] = machine;
    QVERIFY(!MachineConfig::from_json(invalid_tlb, &error).has_value());
    QVERIFY(error.contains("power of two"));

    for (const auto *field : { "btb_bits", "bhr_bits", "bht_address_bits" }) {
        QJsonObject invalid_predictor_size = config.to_json();
        machine = invalid_predictor_size["machine"].toObject();
        predictor = machine["branch_predictor"].toObject();
        predictor[field] = 9;
        machine["branch_predictor"] = predictor;
        invalid_predictor_size["machine"] = machine;
        QVERIFY(!MachineConfig::from_json(invalid_predictor_size, &error).has_value());
        QVERIFY(error.contains(field));
    }
}

QTEST_APPLESS_MAIN(TestMachineConfig)

#include "machineconfig.test.moc"
