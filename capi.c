/* Demo code based on https://github.com/ethereum/evmjit/examples/capi.c */

#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <yaml-cpp/yaml.h>

#include "evm.h"

struct evm_instance* evm_create();

struct evm_uint256be balance(struct evm_context* context,
                             const struct evm_address* address)
{
    struct evm_uint256be ret = {.bytes = {1, 2, 3, 4}};
    return ret;
}

struct evm_address address(struct evm_context* context)
{
    struct evm_address ret = {.bytes = {1, 2, 3, 4}};
    return ret;
}

static void print_address(const struct evm_address* address)
{
    int i = 0;
    for (i = 0; i < sizeof(address->bytes); ++i)
        printf("%x", address->bytes[i] & 0xff);
}

static int account_exists(struct evm_context* context,
                           const struct evm_address* address) {
    printf("EVM-C: EXISTS @");
    print_address(address);
    printf("\n");
    return 0;
}

static void get_storage(struct evm_uint256be* result,
                        struct evm_context* context,
                        const struct evm_address* address,
                        const struct evm_uint256be* key)
{
    printf("EVM-C: SLOAD @");
    print_address(address);
    printf("\n");
}

static void set_storage(struct evm_context* context,
                        const struct evm_address* address,
                        const struct evm_uint256be* key,
                        const struct evm_uint256be* value)
{
    printf("EVM-C: SSTORE @");
    print_address(address);
    printf("\n");
}

static void get_balance(struct evm_uint256be* result,
                        struct evm_context* context,
                        const struct evm_address* address)
{
    printf("EVM-C: BALANCE @");
    print_address(address);
    printf("\n");
    *result = balance(context, address);
}

static size_t get_code(const uint8_t** code,
                       struct evm_context* context,
                       const struct evm_address* address)
{
    printf("EVM-C: CODE @");
    print_address(address);
    printf("\n");
    return 0;
}

static void selfdestruct(struct evm_context* context,
                         const struct evm_address* address,
                         const struct evm_address* beneficiary)
{
    printf("EVM-C: SELFDESTRUCT ");
    print_address(address);
    printf(" -> ");
    print_address(beneficiary);
    printf("\n");
}

static void call(struct evm_result* result,
                 struct evm_context* context,
                 const struct evm_message* msg)
{
    printf("EVM-C: CALL (depth: %d)\n", msg->depth);
    result->status_code = EVM_FAILURE;
}

static void get_tx_context(struct evm_tx_context* result, struct evm_context* context)
{

}

static void get_block_hash(struct evm_uint256be* result, struct evm_context* context,
                           int64_t number)
{

}

/// EVM log callback.
///
/// @note The `evm_log` name is used to avoid conflict with `log()` C function.
static void evm_log(struct evm_context* context, const struct evm_address* address,
                    const uint8_t* data, size_t data_size,
                    const struct evm_uint256be topics[], size_t topics_count)
{
    printf("EVM-C: LOG%d\n", (int)topics_count);
}

static const struct evm_context_fn_table ctx_fn_table = {
    account_exists,
    get_storage,
    set_storage,
    get_balance,
    get_code,
    selfdestruct,
    call,
    get_tx_context,
    get_block_hash,
    evm_log
};

/// Example how the API is supposed to be used.
int main(int argc, char *argv[]) {
    struct evm_instance* jit = evm_create();
    if (jit->abi_version != EVM_ABI_VERSION)
        return 1;  // Incompatible ABI version.


//    uint8_t const code[] = "\0asm\1Place some EVM bytecode here";
/*    uint8_t const code[] =
	"\x00\x61\x73\x6d\x01\x00\x00\x00\x01\x08\x02\x60\x01\x7e\x00\x60"
	"\x00\x00\x02\x13\x01\x08\x65\x74\x68\x65\x72\x65\x65\x6d\x06\x75"
	"\x73\x65\x47\x61\x73\x00\x00\x03\x02\x01\x01\x05\x03\x01\x00\x01"
	"\x07\x08\x01\x04\x6d\x61\x69\x6e\x00\x01\x0a\x09\x01\x07\x00\x42"
	"\x9a\x05\x10\x00\x0b";
*/

    std::ifstream fin("tests/usegas.yml");
    YAML::Parser parser(fin);

    YAML::Node doc;
    while(parser.GetNextDocument(doc)) {
      cout << "something...";
    }
    
uint8_t code[2048];
int fd = open("./wasm/aa.wasm", O_RDONLY);
if (fd < 0)
  abort();
size_t code_size = read(fd, code, sizeof(code));

//    const size_t code_size = sizeof(code);

    struct evm_uint256be code_hash = {.bytes = {1, 2, 3,}};
    uint8_t const input[] = "Hello World!";
    struct evm_uint256be value = {{1, 0,}};
    struct evm_address addr = {{0, 1, 2,}};
    int64_t gas = 200000;

    struct evm_context ctx = {&ctx_fn_table};

    struct evm_message msg = {addr, addr, value, input, sizeof(input),
                              code_hash, gas, 0};

    struct evm_result result =
        jit->execute(jit, &ctx, EVM_HOMESTEAD, &msg, code, code_size);

    printf("Execution result:\n");
    if (result.status_code != EVM_SUCCESS) {
      printf("  EVM execution failure: %d\n", result.status_code);
    } else {
        printf("  Gas used: %" PRId64 "\n", gas - result.gas_left);
        printf("  Gas left: %" PRId64 "\n", result.gas_left);
        printf("  Output size: %zd\n", result.output_size);

        printf("  Output: ");
        size_t i = 0;
        for (i = 0; i < result.output_size; i++) {
            printf("%02x ", result.output_data[i]);
        }
        printf("\n");
    }

    if (result.release)
        result.release(&result);
    jit->destroy(jit);

    return result.status_code == EVM_SUCCESS;
}
