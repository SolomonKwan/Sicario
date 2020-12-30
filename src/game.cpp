
#include <iostream>
#include <unordered_map>
#include <bitset>

#include "game.hpp"
#include "search.hpp"
#include "movegen.hpp"

namespace Moves {
    std::vector<MovesStruct> ROOK = computeRookMoves();
    std::vector<MovesStruct> BISHOP = computeBishopMoves();
    std::vector<MovesStruct> KNIGHT = computeKnightMoves();
    std::vector<MovesStruct> KING = computeKingMoves();
    std::vector<MovesStruct> CASTLING = computeCastling();
    std::vector<MovesStruct> EN_PASSANT = computeEnPassantMoves();
    std::vector<MovesStruct> DOUBLE_PUSH = computeDoublePushMoves();
    std::vector<std::vector<MovesStruct>> PAWN = computePawnMoves();

    namespace Blocks {
        std::vector<MovesStruct> BISHOP = computeBishopBlocks(); // 64
        std::vector<MovesStruct> ROOK = computeRookBlocks(); // 64
    }
}

namespace Indices {
    const std::vector<std::vector<int>> ROOK = computeRookIndices();
    const std::vector<std::vector<int>> BISHOP = computeBishopIndices();
}

/**
 * Hashes used for zobrist.
 */
namespace Hashes {
    Hash PIECES[12][64] = {
        {
            0x7718b18430bb79e4, 0x02cf97689c6446c6, 0x0ed8f775b0a99c1d, 0xe61d1c23cb4df5ca, 0xe5f7624fcb50ee5c,
            0x565173112377b9b3, 0x2e0d7d3511e94224, 0xa99d471a797e781f, 0xe5ef3593ffde7cf4, 0x1ee0ca13d82e185b,
            0xd198ce97a6f3914c, 0x9d809c5671e59963, 0x9af4d22ebc307afa, 0x30ff44ae96261d89, 0x33838dd422fb7abe,
            0xb16fd0e5bca5690a, 0xca5029b2ef4ddd1e, 0x34cebcd1be72ef51, 0xc380e325e462d6d2, 0x78b5b1997d7f76fd,
            0xb88b51eea4357a7d, 0x8885144c4239f3bc, 0x29f723b81221947f, 0x2db27d97e3855c49, 0x6d5cf9c9245a0125,
            0x97a1022431ca02fd, 0xa6470c9a784623f2, 0xe237c95abdbc0f12, 0xdc20f0160ec0a3c4, 0xf98dc042156ca61c,
            0x0f55b35cc0e2e9cb, 0x9f30e2d4e17f4df1, 0x4245607b07e6e622, 0x12635aea42236abd, 0xafdd970038409633,
            0x6dd706a5bd9be8db, 0x7ab3c23c466f3e5d, 0x78e6dacd28c930d4, 0x5d357c6e870f136e, 0x3acf007b787518e2,
            0x44babacf3b2e0f1f, 0x1984455930fa38b9, 0x46f755c93392304a, 0x339afa6214b84254, 0x0b6dc12542267f81,
            0x69177c2784825c39, 0x80a24637e5113964, 0x41acfe3552b73188, 0x54cc92bb4ac50dff, 0xc8adc3c21206fc4f,
            0x44d1ffd36795c39a, 0x8837cc6bda8c65fd, 0x0e676e69714c63fb, 0x2e0750421fe70cb3, 0x7078e358e5e7a498,
            0x5d49559d155686e0, 0x46fccb7f13174424, 0x06c041d3b0abb8bd, 0x1ff4f755a605e18c, 0xa0029f4936138499,
            0xc023be7e16d61b40, 0xddbfd92aaeb8d17b, 0xa8b4d093c7232b5d, 0x23ce14d2198765a6
        },

        {
            0x82f6b6e3dafbcca9, 0xc383bdc31b1b3083, 0x55d23a92f7a023e9, 0xcf48f9a8e2316dbe, 0xda66b066fda9096d,
            0xc58388a8e3fb0244, 0x841a599f40d9561f, 0xfee9284b6622ea1a, 0x1a16c91bd93a170a, 0x3b7abc9abae201a4,
            0x237bd6eaf31111fe, 0x8ba872c8668a3b79, 0x8f652d6a570e5a13, 0x006d1280b6687984, 0x5f104775ba5d6f51,
            0x86e234e3d82ecafb, 0xba6ec97325cad079, 0x5952d977e0b1ee4a, 0xae8df2d853873af7, 0x3e2fed287e84a519,
            0xcb3a8720fa62fd36, 0x830f045dbf7fec93, 0xa6d40f4a087c2674, 0xf2a49d998ec6910d, 0x027075401f869420,
            0x00aca9cdf427e771, 0xb3b23853e70dfd5f, 0x49ec0b932c402f7b, 0x9ba033bd928881c6, 0x4c9385ece359d077,
            0xcd396e3962b53848, 0xaafd7111b45918f8, 0x81ddacaac83275d3, 0x2bdad3c5302d6e89, 0x0250ca1c4ef9f203,
            0x4900e2797b0ae46a, 0x538e092558dd12be, 0xe4548b05c4c15d35, 0xcd76367be82a9bde, 0xfc5a1d8ddf63f5b6,
            0x8e3487caf03561bc, 0x40fb6d002ee94f15, 0x776116be29c307a3, 0xabe0bc4ca81fd531, 0x1458f1e36cc89e6f,
            0x8c0b1480a2eeedd3, 0x9ddedd8e3a0c5ecf, 0x62ad132bd1cb5026, 0x6dff6acd00bf1def, 0x0c8c3004cb29ca7d,
            0x2579bbe85dd23ab2, 0x1bc014fc4a6981cb, 0xea8ce6e0b4af1e0f, 0xce00f7d6e1b68ba2, 0x061ced72c4fac274,
            0x8bcd2f847633106d, 0x221aacfe8dee1ebb, 0xb539a149f1cf5c5d, 0xf05bf33fe51c62b8, 0x980c46e5f0776e8d,
            0xf325a0aa967fd5ff, 0xc616088cd011b830, 0x5937c30643a7ed86, 0x0d48085f7c2bc7df
        },

        {
            0x5328a07129193744, 0x0853fe3008d13fdb, 0x18d91bda5449aa41, 0x845be15f283523f6, 0xf82a1f80ff7ca37b,
            0x7f3deacdf9c292f4, 0x47e570ff2ef0a22c, 0x5041c5dc538217fa, 0x8b730d528371d345, 0x1e58c569eeecc96c,
            0x7fbcbdb110fbd213, 0x4ffe53d896d49afb, 0x59b8a07ef925bc98, 0xa9023f3ae318a626, 0xab16b1a0213a96ac,
            0x5cde241f97da4cb7, 0x9b1eaca45fc3fbd0, 0x41bbd705fad4223f, 0xf2104c520a1bda29, 0x94e8e58a61e899ce,
            0x78e6c00bb1a21953, 0xe4bcdc0f9417b3f0, 0xd5a0e373350b4318, 0x4ab3775bc1e56f82, 0x9d7008a27270564c,
            0xfd3e6c11e78934d9, 0xa5f56c6becdaacd3, 0xb286d9c8e79b41c8, 0x0978e4a444fd946d, 0x7bcce4a8414ac21b,
            0xb4b434197cabc514, 0x43c0efa5b515fb1e, 0xa311f0e6bd8729ae, 0xf100114ccb306c90, 0xa12ec9c189430fb9,
            0xac338182bcea60b9, 0xef56d7183eb951f7, 0x78e8ea533590ed74, 0x9c9390e2579e29c5, 0x72cf3157a5e1af71,
            0xe8afee49237b74e2, 0xd2758f365c01186b, 0x4dcd5a72d320e134, 0xf8c4c9cdd967b021, 0xfbf4d8971b0fbacf,
            0xdf400d18f596da96, 0x66a85511950756e2, 0x4012f27557fce3d0, 0x049628cc5a0b05ef, 0x8a4518b8591060d0,
            0x818bc8cdca18f364, 0x547de23ccc1fa912, 0x1a3c13e1b8fee86a, 0x21c5725d4070c15c, 0x79fa21d20a023b47,
            0xa0db3acc3290b52a, 0x650d10d100618839, 0x6a0289175e860c09, 0x4ee084449f1458d9, 0x4ce28aa9bf579b3b,
            0x88504268f37c202e, 0x772205bbfc502f29, 0xac6bdcb252712a77, 0x5dcc3212a38bbd25
        },

        {
            0xa454ba8a188d208c, 0x8acde3a6d7f4155d, 0xbeaecb935b3649f0, 0x57f6544eca24ad36, 0xcc40b9a32149f3c7,
            0x110d107ebb8b8ecd, 0x21de69d54c44e523, 0x5fec1246859b200f, 0x75569559b535ad90, 0x3b6684eef55da4dc,
            0x5b96511027751c16, 0x79e36c2467edf166, 0x435a086798a54777, 0xe4329b73d9f35ae0, 0xd2f97bc1b1a5afa0,
            0x0ddae808266832f0, 0xcfc051b2b3d0e619, 0xf72100b389df5f6e, 0x3a080952d1640e7e, 0xbbe64b56abc01e31,
            0x310ef690d4a703bb, 0x434c20e3a0a6e02a, 0x828f19c898431ec9, 0xa5624884fc9c6136, 0x49966c8d31a6fba1,
            0xf80d0ac4160ba21d, 0x2e9039cec3e077c9, 0xa5e6ed300b0ebf53, 0xe0b4a6d1795d264c, 0x179e5b367516eaa3,
            0xdcbd7fd8c7c8d237, 0xfb6600617612e384, 0x11d8e2655983d058, 0x86d86001033bb446, 0x17418cf3c68d1a66,
            0x11e50c8ada567fe5, 0xc4b12c11c9268df9, 0x4eb3726fe82128cb, 0x61912c9f236b064f, 0xa5400fe590de591b,
            0x6498dda0ebad773e, 0x9b58618031d8423e, 0x8ed419346b390fa3, 0xbb2f06146166adb0, 0xb22689dd83ed7bcf,
            0x4c931b0adb4d26fc, 0xe8b38a94b44c7d92, 0xd62b047eb8703ecd, 0x71cd30b1ea2907b9, 0xbb16cb0ee0ff57a9,
            0x36a8400e88b1e819, 0x218ca6dbde51f964, 0x5af349c07b37f5e9, 0xe84afce3b9803ec9, 0xa1b88164f5d0e3f3,
            0x646170eb07080a49, 0xc924ad4849ede669, 0xed79f27d1634ba66, 0x16e76be9a842bba0, 0x843d4b6972a4cad9,
            0x5741946ca024d14f, 0xb6fad4c66d5665dd, 0x0549eb6e565dc9a5, 0x08b310b6ff31530c
        },

        {
            0xc6071e35c1a8a6e8, 0xc17d282348c3dd35, 0x9745797b7775dc86, 0x163da19f2f71d345, 0xf896c12548656483,
            0x3f7bb7603f5be57c, 0x6306cfd5608a95a0, 0x922d362146e06524, 0x758dc8238b867cb6, 0x70b68b593677a86e,
            0x4c70d453caa4a6ff, 0x466a8c936a5cb383, 0x4bdd8373ab62dbba, 0x043cd4321003e1c4, 0x26c8b0460381b844,
            0x083bcebe88a9547d, 0xa573c887d6cf6037, 0xe4841b2d1ff7decf, 0x01c5832098575e00, 0xb786f843d250eeda,
            0x2a43ce0720ecd0af, 0x5317561af0d180ea, 0x8ddb675dee77bad3, 0x9aa6df563c99da45, 0x8c1d5368510d633a,
            0xcc39822f78b500c7, 0xd02df1ff3f711a1f, 0x08ff3d6d1565c790, 0x85f33cf44d764989, 0xd733c8ebd27ffd63,
            0xafa9d4f972436436, 0xdbf021fa9bd3a24f, 0x4fb0682c5477e7db, 0xfc919755969305ae, 0xb5700a69c980051c,
            0xe1907478f17532b4, 0x8b295efdcb29961c, 0xd60240cb1d4ba4e5, 0xbbd03427b55ab9ca, 0x0bee7adcdacbb5c8,
            0x6380974d5ed1bcee, 0x1d12b43aa45a8d87, 0x391b9130ec927782, 0x7034cd456aeff157, 0xb8fda38a0100282c,
            0xa3e9ddf9d69f3611, 0x2bdebe12cd894fde, 0xa03fb3e364d03f21, 0x5a486f911d10f8a4, 0x8775db169593e296,
            0x11593b424ee4a8c6, 0x16f7a9cb251718b4, 0xb1f1635eead84fa8, 0x612320517bfc393c, 0x6f915259026e6bd5,
            0x5e4aec7dadc9e612, 0xb2f052099598562e, 0x3a1063ff6fed8b03, 0x288337e2dcde088f, 0xf7848f2c668bb823,
            0xd67e6c9ad9e67190, 0x0b5580723da59228, 0x908957d5faf07393, 0xb8a16ed0d7221626
        },

        {
            0x36084c93a54839b9, 0xc1095e7bfe2ae5f1, 0x10cac2b299ef0ae0, 0x6f538a591522f269, 0x7a066f22d7622f6d,
            0x2b76cf9d7783d61d, 0xfc8a06356c65f414, 0x460942f5e81b0dc4, 0x08d26fb912b0a8a9, 0x2e3e9bee85dedef2,
            0xb38c195413b63afd, 0x4e5ebdd50bc073db, 0x241ca5b991aaee95, 0xb20adb3bca3a3007, 0x30f89139c622e0b7,
            0x125584d78556174d, 0x22d86a299a6d6960, 0x9c38fe364b84de24, 0xd0d45ca3ae2241d5, 0x40474ddf34391912,
            0x50741117c43278ab, 0xc45007ab56131e6e, 0xd85e85a1fa09eff3, 0xfb415bb5232c0072, 0x5d88da08af77671d,
            0xb817d85710753e95, 0xc11fc256a13b85f0, 0x58831ab6ea1bee6e, 0x79ecfb0a525a3c28, 0x927232b238490608,
            0x2221703a4cbd2ca3, 0x1def9effbfe6f005, 0xc1c26250e1caadc0, 0xaa6eccb3072496b7, 0x566c68840701e908,
            0x1a7e1455ba34a1f7, 0x9134a72ac6429f75, 0xd3e22b9bef9ef27b, 0x9c52c6ad38919790, 0x628414848f0c7572,
            0xf4d8539842d69310, 0xb3bafb37e2023f3e, 0x00ef504e2a211300, 0xdfaaed3847729090, 0x0e3a950de09d2829,
            0x1780def5815393cb, 0x523b825f347edcc0, 0xed592185c71334ef, 0x95960b76c9c9e6ae, 0xa030e302d8164480,
            0xcbb038fb54e0caf4, 0xb311de2ee754b0c5, 0xa14c1cbb80054c56, 0x8ce94405f0697c03, 0x3d0dace177f0adb8,
            0xa932123b8f2be57a, 0xa6a51113bb6e0ef7, 0x7171f3ad354bc43b, 0xace7545008fa6093, 0x5af2f0009bef5dc8,
            0xe2270e960c48a1b9, 0xa3e53b402da221d4, 0x29a26e91aa3df131, 0xba17397aae41713d
        },

        {
            0xbc47f0a8238d87f0, 0x937eb92bb4bf0813, 0x12560ed4adc3d3e1, 0x9b0994f52f0e0486, 0x7625c12d56320c85,
            0x3ae05cd9051ac971, 0xb635c573afc68a39, 0x510da52584d99aca, 0x1f564b57bb07ddc4, 0xaa776e7713ffd80b,
            0x2e2cf8c32c7b0a30, 0x7a07a3e159fef032, 0x12fbcc650d35d437, 0x68017907cef10934, 0x7ad5cc81cf939f20,
            0x583609a9132deb28, 0xf4ab1adcacc24e71, 0xe42dc99dbf71cab9, 0x572fd9e559380702, 0x17093078fbcb67f8,
            0x6306b5e06ab862a0, 0xb893ad12f40ff5e5, 0x968f022799b85d06, 0xd5e9fa26d952402e, 0x55d93f20eb663238,
            0xead0023b0cb2afb6, 0xd284d482b0b915a6, 0x3fdfa21ca259d3b0, 0x49ea437ebffa394d, 0xa6d70e19489f87e8,
            0x9cd3c81fe51cdbea, 0x94b2163d5ca74b57, 0xfc3beeb3baa33b34, 0x976b15bad867c103, 0x6fa776311b4c2793,
            0xecaa9f03662959d7, 0xd6ff97218b2f632b, 0x5c9ec6ccdc9ea457, 0x53a7f92e85e27a64, 0xb83a018e55416e88,
            0x80994211be5ed2c1, 0x11fb7e9e979d986e, 0xdbfd94a31f30f44e, 0x4371663e31c4019c, 0xdbf4d9afda17d299,
            0x76244dfc31458e62, 0xf369b78dbd560e48, 0x2bb2e9b330eec908, 0xdb01e0fe14e6454d, 0xe9173709fdd95c3b,
            0x6ad9fe2e9cab3594, 0x435497c3b8c536bc, 0xc22ec6b299dc3951, 0x0df12756ad12a891, 0x380c00e92ee77a0e,
            0x1b598b02c4eeb16f, 0x865157ddf040240e, 0x1470c3fbb891bb5f, 0xe87bc276848c48a9, 0x3d4fe73dd8bcf4d5,
            0x138863f4440e8086, 0x5ba3db45fa7a7bf6, 0x6a691605de9be820, 0x8781844541fb9d6c
        },

        {
            0x46d1eca8ab723c9c, 0x64cf23cfcee69bfb, 0xad64340e549c4cb5, 0xfcf3687d678b20f7, 0xaacd98788b191d4f,
            0x8785e0917fa3b986, 0x47f669ed811b2c52, 0x29b311595217156f, 0x0bebf463e6867daf, 0xb918aff482bc94cf,
            0xeac19987cce23848, 0x024f95f59ee3cf18, 0x07e557433dd4eb0a, 0xc30579e61ada5482, 0xa8346d1a7fa6cf05,
            0xba8dd9e5a0305469, 0x329d393dda489097, 0x1b81a49cb3ec5ca6, 0x61765144fb039396, 0x06d1883aa4f8f95d,
            0x5d34e49ea1461c69, 0xba28280a9e9ca78e, 0x67d3f06192745fa7, 0x6f2f529de4aae463, 0x845d2dead7594525,
            0x5d97f0ebd5823a5d, 0x702a679b6b6961fe, 0x1bd39548eac7c64a, 0x87850eaca8ad8df2, 0x5a54969ae417e256,
            0x5fbd018b947d0bf5, 0x1de080d5e3426f15, 0x66c82be1e1cc9e2b, 0xd95ca996d035b9bd, 0x10f61823137a69bd,
            0x0335c71eaec584b1, 0x164ea82c7c2892e0, 0x4e336835eb48f4af, 0x0e0638591d016e3a, 0xe5c5d8bf451ad482,
            0x11138e9fcd102961, 0xb51ea10d1f088f67, 0xd852fa943506ada7, 0x8a67301a341bd92a, 0x3bad95689eeb35c2,
            0x2d4f118b48d4d0c0, 0x482457b42ab3be95, 0xb474beac95d68465, 0x57d5c85e6e02fadb, 0xd1332374576761d7,
            0xbc34976f1ccd952e, 0xdedf02b1808b1580, 0x15804e8bf640120b, 0x1838280478c08d9e, 0x795d52e0a36d5eb1,
            0x7d709f017e49f4c5, 0x06c342350f7e0cc3, 0x05d579cf4b71e184, 0x27119a549056c279, 0x9236fca7916db544,
            0xaef42faf75720e87, 0x763146f26101a7ef, 0xf20dc7861dfd176c, 0xb8b4a1e0a99d4ea9
        },

        {
            0x76b10a4b26c406e0, 0xa630f66b7040734f, 0xe44ee804a2431036, 0x2f62f6610523b96a, 0x4fee979f02f65316,
            0x541ae4c112c0a1ff, 0xd6f706266875697b, 0x5d9d2e7daab553d6, 0x7550c3f2b0f00fb6, 0x6412a4c2ffa0f91e,
            0x9ffc39d254a23bc9, 0x71be0e19904afe59, 0x3b73eb7b73f30195, 0xeadeff1a53c6bf5b, 0xfa5c9ca08304e793,
            0xb960b48c9730bc32, 0xe58564536f856fc2, 0x827ff61d3ff6173d, 0x55b9e982567b571b, 0xd63393ff2c7b489f,
            0xbce4f208898ec32b, 0xab3ed9e4c584bf04, 0xc44370f4b0803967, 0x3720c7e7f4ba8511, 0x51c3f1200f42ff74,
            0xd84ba6d4d4b78633, 0xa969652aa1c7c68a, 0x65d0553fe8379d25, 0x9d496f74416d91d4, 0xa083f7040afbc196,
            0xa873287f17fbf66a, 0x42890b8348bd95be, 0x0e625adf1455ccc1, 0x76f8278af1703445, 0xfc3ff72da781a9ea,
            0x65516c8406d63b4f, 0x1f9cc14a0f128d69, 0xdf08a621d1c0ff11, 0x0cb0a5955e2df749, 0x2b207cd18c3999b5,
            0x7ced87e2cd8b84ce, 0xb39204e91e562ec6, 0xd3879476743ecd47, 0xf713548cb980614d, 0x7680b6a03273d14b,
            0xc962ca24ca8dcee2, 0x8d562128daa8deac, 0x81b07e8ef925183d, 0xa89be8a59859ede4, 0xbaa08c2061f02984,
            0x3b5f3a6a7d451abb, 0x57d43bb0147736f1, 0x56eefa761c8e5c94, 0xdb65c5578cc26352, 0x874f3070e72b7cd0,
            0x2275c6a880f18a60, 0x4413e7e4eb26ecf3, 0xf9870f6384014c1f, 0x3fa5dfdee3738ccc, 0xdb4eda7481ce2334,
            0xa3352340d00f7748, 0x92f20a9e5e71d212, 0x556e88af78346233, 0xb6cec8a1c24d4ead
        },

        {
            0xc5ba00851598df8e, 0xcd1ef2cc3b9584ca, 0xe0c70e5d6f8da116, 0xf7318d278909bfb9, 0x6b12286884d8a2cb,
            0x4d0bd1e289f9b683, 0x3854bfa03f7640ba, 0xd464d3317ca61098, 0x64216030c6c0ba39, 0x4737fd74812e8744,
            0x8ac98a54fb71aa5e, 0x2851ad90572bb046, 0x21ba4be55c6bbbd9, 0x9d9ef28373e89027, 0xd62ce2065d9207a9,
            0xe5f1ad3ad593582a, 0xb5b0a351fb19a511, 0x9999751727c4dcb5, 0x01b478675cb39751, 0xab19087787825263,
            0x5e53b67014b7cff0, 0x535fd8590d234e60, 0xcefa502e3d806538, 0x529fa7d06e974230, 0xe59eb549a5975764,
            0xac5daee86d52d146, 0xda974179498ed360, 0x49cfb47b958758a0, 0x48c60b78f4cbd34a, 0xdfc170d7f4a3bf92,
            0x0ce31041bf6aadf5, 0xbc6c99d52e9bf956, 0xbc515f59106e1aae, 0x33078f7ef10f6b04, 0xfed89962cfd686cf,
            0x5ea3c7ae88f3701e, 0x069b207c98bedf69, 0xf0f0b42404c2760f, 0x03374d220abb417f, 0x9adb341342d71feb,
            0x1bcfd57eabc4870c, 0x2c269a871f261cdd, 0x42c27ef633f1e844, 0xa219900b2c922081, 0xc6a3fa2b7d62e112,
            0xe6a4322670c9c176, 0x0025b9f768238d41, 0x7b28ca179216dbbb, 0xf27dce96cfcfceee, 0xcbbd5c2f233a5c9a,
            0x2f8d30f512dc75ca, 0xba9cd9c3fdfb7587, 0x8ab5d968151f09bf, 0x35ad4d7f000ef8af, 0x080efbbcae269412,
            0x5cc4be8519b93629, 0x87b683ef075a0293, 0x4bb08555859537fc, 0xfdf0604ce4d90660, 0xeed1e75cfbeda32b,
            0x92d9a1f7afa6dcd2, 0x29085e2b32597aee, 0x5de08e834f05cbb9, 0x0b23be4d70a5fe91
        },

        {
            0x19d25ea725575a67, 0x17cf56c79bffb5e4, 0x562392017d151c9d, 0xf14bf82a611163c8, 0x98855eea733c43a0,
            0x511d649c3903b1a5, 0xc9e56e22e5879d27, 0xd855086b975b425e, 0x962451f58ecfb94b, 0x7fb04e5d1388d669,
            0x484e9f9131ff8844, 0xa55c74729af5f9d7, 0x7dc94490e8c85ae5, 0xe2c68089902cba5b, 0x0215a74604e5a9d2,
            0xa12ffd8501b955ff, 0x32d8e745371c7f6d, 0x50f7e055b159356a, 0xf8a38bd7766466f4, 0x2ef01210a7e08627,
            0x4fdcb88dc9a41ec4, 0x4b1976d4fa5fb987, 0xf6b48884a1885938, 0xc3ff46dfab84c2a9, 0x8b520628970facc1,
            0xb891e786a7989cae, 0x68128f7250fec0b1, 0xf7e8bbc12ce41c86, 0x632d56b870c8224a, 0xb2bb53463a3f0c02,
            0xac34f6545e19a0cd, 0x09101d47221a13b7, 0x3b7b0552bc10514b, 0xde496e6cf0b6e445, 0x45b75b8abf1194a0,
            0xe0d3f510c210e393, 0x27d6e783147d7a3e, 0x8ef294380908bfb8, 0xff35068c697119fc, 0x3ce88c55db9a74d3,
            0x7b982f0e56fe43de, 0x212b8b640a733d36, 0x5113bf7d1742bc52, 0x10f284997fe0a475, 0xf4ff5a259c7928af,
            0x85093afd09421c53, 0x0a83010a91319c36, 0xf9f2eed7f5d613d5, 0x45ce9123c6ce24e3, 0xce0ab5cea2a34e04,
            0x1b63e2c7c359a9b4, 0x83e07c31f27137c3, 0x97891a4061e8700d, 0x1372b33067724f44, 0x2d57f98cc69d50ab,
            0xd988461884c97cda, 0xbab098be50ccac2b, 0x8501b4bf6d5070be, 0x7471499badde1668, 0x53faf50aef652027,
            0xf34001e6c5fe75e5, 0xda340122d31606ae, 0x664b2c03d60d3dfa, 0x912527ce791d80c9
        },

        {
            0x5dd3cd6dd7129374, 0x107d4ec15d0fa4c5, 0xf03c53d8458a35b8, 0x89a94cd827df9d40, 0x1c991aff5dde841f,
            0xbde32bd5b202be8d, 0xfa996807a5806647, 0xdfd652af50ab80ed, 0x05d354449b964ec0, 0x5761b48aaa6c37a1,
            0xd45137f601dbf4c4, 0xd62e9212a3ab6e75, 0x1f6d252b4e023529, 0xb2f94cf8b04c408e, 0xbf32ead5ae3680ab,
            0x8b8b0f37874bc50b, 0x892961276f4548a6, 0xa8e08fb1ced99a5f, 0xec06fd67e0181499, 0x2536d9c6595b622b,
            0xa454f9bde52ec9fd, 0x220424a29c662516, 0x8b86988c9a610db1, 0x54cb4dd6849c9fd8, 0xe60e2543ee43a7f3,
            0xb06653f36304feae, 0x47cdf6a41f807982, 0x2ff01dd818ba2af3, 0x6b891259296ef9ee, 0xc3e06d1a3cc054b1,
            0x1da72bbda7c540c7, 0x46cec87887267b07, 0x4d1d843eb3d20bba, 0x628a4ee6b8a602c7, 0xc270f1b4ad688741,
            0xafd3e623df0a23a7, 0x3589a5d4ffa027d4, 0xe20e9f3706840e2d, 0xe0c04717609f1a8c, 0x09202003b5914d37,
            0x41d481b289ccf40f, 0x5d261b9e4cbb2d9b, 0xdca661a694ab236f, 0xd7dcd733b22ab5d7, 0x3c847f81e02b27c2,
            0xd118ce85101673fb, 0xb35507dcc0c055e8, 0x7bf57a8dfb1f1c73, 0x1bd0a05d83d175ac, 0x3e7619fe29e286c0,
            0xa73bd562741768ec, 0xf3063d9e204c526c, 0xf3b6ee89ff3533eb, 0xa8e05d2bfafc577e, 0xfa22b5d2f4df6550,
            0xee5a8e3370715130, 0x36cb6ed13d7a3656, 0xe918462a089545f3, 0xc6b2209fe070ed8a, 0x885ffde1301c962f,
            0xc6a34ead5fd10d0b, 0x8581f9020d0be57b, 0x5b0e708b887e9723, 0xab9d02d44b80c023
        },
    };
    Hash TURN = 0x5c3b287ef7fb458c;
    Hash CASTLING[16] = {
        0xda717836ed8fd501, 0x07c1b72374c36ffc, 0x23bb5aaec6183b2c, 0xc0f3561018b90d2d, 0xb588f4030c37182f,
        0x0ae78cdd96e708f9, 0x30e513338fccbbd2, 0x5653d7721b395a87, 0x92637def2c813b6f, 0x55bb54c7c33a135d,
        0x0468df8b899a8696, 0x590f45d7c2aaa2a5, 0xe0096f3be2c7c8e1, 0x9004d434bee026b3, 0xbe2abac134719858,
        0x08ee593853a48375
    };
    Hash EN_PASSANT[8] = {
        0xe41f7c2010520212, 0x42d91118e6aedac6, 0xd85ccd9fb3f08313, 0x6dcb4caf0a197bec, 0xac0a52f3b29311f8,
        0x97dd3bd5ae77b4a4, 0xedff8a0d047310af, 0xac6b72801c49744c
    };
}

/**
 * Return true if a square is a dark square, else false.
 * @param square: The square to check.
 */
bool isDark(int square) {
    if ((square % 2 == 0 && ((square / 8) % 2) == 0) || (square % 2 == 1 && ((square / 8) % 2) == 1)) {
        return true;
    }
    return false;
}

/**
 * Splits a string by a delimeter string and returns vector of results.
 * 
 * @param input: String to split.
 * @param delim: String delimiter.
 * 
 * @return: Vector of input split by delim.
 */
std::vector<std::string> split(std::string input, std::string delim) {
    std::vector<std::string> result;
    std::size_t pos;

    while ((pos = input.find(delim)) != std::string::npos) {
        result.push_back(input.substr(0, pos));
        input = input.substr(pos + delim.length(), input.length() - 1);
    }
    result.push_back(input.substr(0, pos));

    return result;
}

/**
 * Display all individual game position information.
 * @param game: Pointer to game struct.
 */
void Pos::displayAll() const {
    std::cout << "White";
    displayBB(this->sides[WHITE]);

    std::cout << "Black";
    displayBB(this->sides[BLACK]);

    std::cout << "Kings";
    displayBB(this->kings);

    std::cout << "Queens";
    displayBB(this->queens);

    std::cout << "Rooks";
    displayBB(this->rooks);

    std::cout << "Bishops";
    displayBB(this->bishops);

    std::cout << "Knights";
    displayBB(this->knights);

    std::cout << "pawns";
    displayBB(this->pawns);

    std::cout << (this->turn ? "White" : "Black") << '\n';

    std::cout << "Castling: " << std::bitset<4>(this->castling) << '\n';

    std::cout << "En-passant: " << squareName[this->en_passant] << '\n';

    std::cout << "Halfmove: " << this->halfmove << '\n';
    
    std::cout << "Fullmove: " << this->fullmove << "\n\n";

    std::cout << "piece_cnt: " << this->piece_cnt << '\n';
    std::cout << "knight_cnt: " << this->knight_cnt << '\n';
    std::cout << "wdsb_cnt: " << this->wdsb_cnt << '\n';
    std::cout << "wlsb_cnt: " << this->wlsb_cnt << '\n';
    std::cout << "bdsb_cnt: " << this->bdsb_cnt << '\n';
    std::cout << "blsb_cnt: " << this->blsb_cnt << "\n\n";

    for (int i = 0; i < 12; i++) {
        std::cout << piece_type_string[i] << " " << this->piece_index[i] << " : ";
        for (int j = 0; j < this->piece_index[i]; j++) {
            std::cout << squareName[this->piece_list[i][j]] << " ";
        }
        std::cout << '\n';
    }
    std::cout << '\n';

    for (int i = 0; i < 64; i++) {
        std::cout << squareName[i] << ":" << piece_type_string[this->pieces[i]] << " ";
        if (i % 8 == 7) std::cout << '\n';
    }
}

/**
 * Returns whether or not a fen is valid. STILL TO BE MADE...
 * 
 * @param fen: The fen string to check.
 * 
 * @return: True if valid, else false.
 */
bool goodFen(std::string fen) {
    return true;
}

/**
 * Parses the fen string into a game struct.
 * 
 * @param game: Pointer to game struct.
 * @param fen: The fen string.
 * 
 * @return: INVALID_FEN if fen is invalid, else NORMAL_PLY.
 */
ExitCode Pos::parseFen(std::string fen) {
    if (!goodFen(fen)) return INVALID_FEN;

    // Zero out variables.
    this->zero();

    // Split the string into parts and ranks
    std::vector<std::string> parts = split(fen, " ");
    std::vector<std::string> ranks = split(parts[0], "/");

    // Set the pieces
    int rank = 7;
    for (std::string rank_string : ranks) {
        int file = 0;

        for (char c : rank_string) {
            if (std::isdigit(static_cast<unsigned char>(c))) {
                for (int i = 0; i < (c - '0'); i++) {
                    this->pieces[8 * rank + file] = NO_PIECE;
                    file++;
                }
            } else {
                if (c == 'K') {
                    this->kings |= 1ULL << (8 * rank + file);
                    this->piece_list[WHITE][0] = (Square) (8 * rank + file);
                    this->piece_index[W_KING]++;
                    this->pieces[8 * rank + file] = W_KING;
                } else if (c == 'Q') {
                    this->queens |= 1ULL << (8 * rank + file);
                    this->piece_list[W_QUEEN][this->piece_index[W_QUEEN]] = 
                            (Square)(8 * rank + file);
                    this->piece_index[W_QUEEN]++;
                    this->pieces[8 * rank + file] = W_QUEEN;
                } else if (c == 'R') {
                    this->rooks |= 1ULL << (8 * rank + file);
                    this->piece_list[W_ROOK][this->piece_index[W_ROOK]] =
                            (Square)(8 * rank + file);
                    this->piece_index[W_ROOK]++;
                    this->pieces[8 * rank + file] = W_ROOK;
                } else if (c == 'B') {
                    this->bishops |= 1ULL << (8 * rank + file);
                    this->piece_list[W_BISHOP][this->piece_index[W_BISHOP]] =
                            (Square)(8 * rank + file);
                    this->piece_index[W_BISHOP]++;
                    this->pieces[8 * rank + file] = W_BISHOP;
                    if (isDark(8 * rank + file)) {
                        this->wdsb_cnt++;
                    } else {
                        this->wlsb_cnt++;
                    }
                } else if (c == 'N') {
                    this->knights |= 1ULL << (8 * rank + file);
                    this->piece_list[W_KNIGHT][this->piece_index[W_KNIGHT]] =
                            (Square)(8 * rank + file);
                    this->piece_index[W_KNIGHT]++;
                    this->pieces[8 * rank + file] = W_KNIGHT;
                    this->knight_cnt++;
                } else if (c == 'P') {
                    this->pawns |= 1ULL << (8 * rank + file);
                    this->piece_list[W_PAWN][this->piece_index[W_PAWN]] =
                            (Square)(8 * rank + file);
                    this->piece_index[W_PAWN]++;
                    this->pieces[8 * rank + file] = W_PAWN;
                } else if (c == 'k') {
                    this->kings |= 1ULL << (8 * rank + file);
                    this->piece_list[BLACK][0] = (Square) (8 * rank + file);
                    this->piece_index[B_KING]++;
                    this->pieces[8 * rank + file] = B_KING;
                } else if (c == 'q') {
                    this->queens |= 1ULL << (8 * rank + file);
                    this->piece_list[B_QUEEN][this->piece_index[B_QUEEN]] =
                            (Square)(8 * rank + file);
                    this->piece_index[B_QUEEN]++;
                    this->pieces[8 * rank + file] = B_QUEEN;
                } else if (c == 'r') {
                    this->rooks |= 1ULL << (8 * rank + file);
                    this->piece_list[B_ROOK][this->piece_index[B_ROOK]] =
                            (Square)(8 * rank + file);
                    this->piece_index[B_ROOK]++;
                    this->pieces[8 * rank + file] = B_ROOK;
                } else if (c == 'b') {
                    this->bishops |= 1ULL << (8 * rank + file);
                    this->piece_list[B_BISHOP][this->piece_index[B_BISHOP]] =
                            (Square)(8 * rank + file);
                    this->piece_index[B_BISHOP]++;
                    this->pieces[8 * rank + file] = B_BISHOP;
                    if (isDark(8 * rank + file)) {
                        this->bdsb_cnt++;
                    } else {
                        this->blsb_cnt++;
                    }
                } else if (c == 'n') {
                    this->knights |= 1ULL << (8 * rank + file);
                    this->piece_list[B_KNIGHT][this->piece_index[B_KNIGHT]] =
                            (Square)(8 * rank + file);
                    this->piece_index[B_KNIGHT]++;
                    this->pieces[8 * rank + file] = B_KNIGHT;
                    this->knight_cnt++;
                } else {
                    this->pawns |= 1ULL << (8 * rank + file);
                    this->piece_list[B_PAWN][this->piece_index[B_PAWN]] =
                            (Square)(8 * rank + file);
                    this->piece_index[B_PAWN]++;
                    this->pieces[8 * rank + file] = B_PAWN;
                }

                // Set sides and piece count
                if (std::isupper(static_cast<unsigned char>(c))) {
                    this->sides[WHITE] |= 1ULL << (8 * rank + file);
                } else {
                    this->sides[BLACK] |= 1ULL << (8 * rank + file);
                }
                this->piece_cnt++;

                file++;
            }
        }

        rank--;
    }

    // Set turn
    if (parts[1] == "w") {
        this->turn = WHITE;
    } else {
        this->turn = BLACK;
    }

    // Set castling
    this->castling = 0;
    if (parts[2] != "-") {
        for (char c : parts[2]) {
            if (c == 'K') {
                this->castling |= 1 << WKSC;
            } else if (c == 'Q') {
                this->castling |= 1 << WQSC;
            } else if (c == 'k') {
                this->castling |= 1 << BKSC;
            } else {
                this->castling |= 1 << BQSC;
            }
        }
    }

    // Set en-passant
    if (parts[3] != "-") {
        int value = -1;
        for (char c : parts[3]) {
            if (value == -1) {
                value = (c - 'a');
            } else {
                value += 8 * (c - '1');
            }
        }
        this->en_passant = (Square) value;
    } else {
        this->en_passant = NONE;
    }

    // Set fullmoves and halfmoves.
    this->halfmove = std::stoi(parts[4]);
    this->fullmove = std::stoi(parts[5]);

    // Set ply and position hash to zero.
    this->ply = 0;
    this->hash = 0ULL;

    return NORMAL_PLY;
}

void Pos::zero() {
    this->sides[WHITE] = 0ULL;
    this->sides[BLACK] = 0ULL;
    this->kings = 0ULL;
    this->queens = 0ULL;
    this->rooks = 0ULL;
    this->bishops = 0ULL;
    this->knights = 0ULL;
    this->pawns = 0ULL;

    std::fill(this->piece_list[0] + 0, this->piece_list[12] + 10, NONE);
    std::fill(std::begin(this->piece_index), std::end(this->piece_index), 0);
    std::fill(std::begin(this->pieces), std::end(this->pieces), NO_PIECE);

    this->piece_cnt = 0;
    this->knight_cnt = 0;
    this->wdsb_cnt = 0;
    this->wlsb_cnt = 0;
    this->bdsb_cnt = 0;
    this->blsb_cnt = 0;
}

/**
 * Show the usage of the program.
 * 
 * @param argv: Array of command line arguments.
 */
void showUsage(char *argv[]) {
    std::cerr << "\nUsage: " << argv[0] << " white black [options]\n";
    std::cerr << "    white - Type of player for white (c or h)\n";
    std::cerr << "    black - Type of player for white (c or h)\n\n";
    std::cerr << "Options:\n";
    std::cerr << "    -f FEN            FEN input string in quotes (' or \").";
    std::cerr << "\n    -h --help         Print usage information.\n";
    std::cerr << "    -i                Invert piece colours.\n";
    std::cerr << "    -p                Print pgn.\n";
    std::cerr << "    -q                Supress output.\n";
    std::cerr << "    -u                Show unicode chess chars.\n";
    std::cerr << "    --version         Show version.\n\n";
    std::cerr << "Compiled by running 'make'.\n";
    std::cerr << "Made in development using GNU Make 4.1.\n\n";
}

/**
 * Show the end of game message.
 * @param code: The exitcode of the game.
 * @param argv: Array of command line arguments.
 */
void Pos::showEOG(ExitCode code) {
    switch (code) {
        case NORMAL_PLY:
            std::cout << "Debugging: normal\n";
            break;

        case WHITE_WINS:
            std::cout << "Checkmate, white wins\n";
            break;
        
        case BLACK_WINS:
            std::cout << "Checkmate, black wins\n";
            break;
        
        case STALEMATE:
            std::cout << "Draw by stalemate\n";
            break;
        
        case THREE_FOLD_REPETITION:
            std::cout << "Draw by three-fold repetition\n";
            break;
        
        case FIFTY_MOVES_RULE:
            std::cout << "Draw by fifty move rule\n";
            break;
        
        case INSUFFICIENT_MATERIAL:
            std::cout << "Draw by insufficient material\n";
            break;
        
        case DRAW_BY_AGREEMENT:
            std::cout << "Draw by agreement\n";
            break;
        
        case INVALID_FEN:
            std::cout << "Invalid FEN string\n";
            break;
        
        case INVALID_ARGS:
            // showUsage(argv);
            break;

        default:
            std::cout << "Should not happen...\n";
    }
}

/**
 * Computes the index (in this case the key) into the ROOK_BLOCK moves map.
 * @param pos: The uint64_t with possible destination squares from the param
 *      square set.
 * @param moves: The precomputed moves.
 * @param square: The square from which the piece moves.
 * @param game: A Game struct pointer.
 * @return: Index into the ROOK_BLOCK array.
 */
const int Pos::rookBlockIndex(uint64_t pos, Square square) {
    return pos & this->getRookFamily(square)->reach;
}

/**
 * Check if draw by insufficient material.
 * 
 * @param game: Pointer to game struct.
 * @return: True if draw, else false.
 */
bool Pos::insufficientMaterial() {
    // King vs king
    if (this->piece_cnt == 2) return true;

    // King and bishop(s) vs king (same colour bishop(s))
    if ((this->wlsb_cnt && (this->piece_cnt - this->wlsb_cnt) == 2) ||
            (this->wdsb_cnt && (this->piece_cnt - this->wdsb_cnt) == 2) ||
            (this->blsb_cnt && (this->piece_cnt - this->blsb_cnt) == 2) ||
            (this->bdsb_cnt && (this->piece_cnt - this->bdsb_cnt) == 2)) {
        return true;
    }

    // King and bishop(s) vs king and bishop(s) (same colour bishop(s))
    if ((this->wlsb_cnt && this->blsb_cnt && (this->piece_cnt - 
            this->wlsb_cnt - this->blsb_cnt) == 2) || (this->wdsb_cnt && 
            this->bdsb_cnt && (this->piece_cnt - this->wdsb_cnt - 
            this->bdsb_cnt) == 2)) {
        return true;
    }

    // King and knight vs king
    if (this->knight_cnt == 1 && this->piece_cnt == 3) return true;

    return false;
}

/**
 * Check if king of current player is in check.
 * @param game: Pointer to game struct.
 * @param enemy_attacks: Bitboard of squares attacked by enemy pieces.
 * @return: True if in check, else false.
 */
bool Pos::isChecked() {
    return (1ULL << this->piece_list[this->turn][0]) & this->enemy_attacks;
}

/**
 * Sets all the legal moves a king can make in a given position.
 * @param game: Pointer to game struct.
 * @param computed_moves: Pointer to precomputed moves struct.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getKingMoves(std::vector<Move>* pos_moves[MAX_MOVE_SETS], int& moves_index) {
    MovesStruct* king_family = &Moves::KING[this->piece_list[this->turn][0]];
    std::vector<Move>* move_set = &king_family->move_set[moveSetIndex((king_family->reach ^ this->sides[this->turn]) & 
            ~this->kEnemy_attacks, king_family)];
    if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;
}

/**
 * Retrieves the legal moves for when the king is in check.
 * @param game: A game struct pointer.
 * @param moves: Precomputed moves struct pointer.
 * @param enemy_attacks: 64 bit unsigned int pointer for computing king moves.
 * @param rook_pins: 64 bit unsigned int pointer for rook pins.
 * @param bishop_pins: 64 bit unsigned int pointer for bishop pins.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getCheckedMoves(std::vector<Move>* pos_moves[MAX_MOVE_SETS], int& moves_index) {    
    // MIGHT BE ABLE TO RETRIEVE WHEN CALCULATING THE enemy_attacks
    Square king_sq = this->piece_list[this->turn][0];
    uint64_t checkers_only = 0ULL;
    uint64_t check_rays_only = 
            this->getBishopCheckRays(king_sq, &checkers_only) | 
            this->getRookCheckRays(king_sq, &checkers_only) | 
            this->getPawnCheckers(king_sq, &checkers_only) | 
            this->getKnightCheckers(king_sq, &checkers_only);
    
    // King checked moves.
    this->getKingMoves(pos_moves, moves_index);

    // Queen checked moves.
    PieceType piece = this->turn ? W_QUEEN : B_QUEEN;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int queen = this->piece_list[piece][i];
        if ((1ULL << queen) & this->rook_pins || (1ULL << queen) & this->bishop_pins) {
            continue;
        }
        MovesStruct* bishop_move = &Moves::BISHOP[Indices::BISHOP[queen][
                bishopIndex(this->sides[BLACK] | this->sides[WHITE], 
                        (Square) queen)]];
        uint64_t bishop_index = bishop_move->reach & (check_rays_only | 
                checkers_only);
        std::vector<Move>* move_set = &Moves::Blocks::BISHOP[queen].checked_moves[bishop_index];
        if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;

        MovesStruct* rook_move = &Moves::ROOK[Indices::ROOK[queen][
                rookIndex(this->sides[BLACK] | this->sides[WHITE], 
                (Square) queen)]];
        uint64_t rook_index = rook_move->reach & (check_rays_only | 
                checkers_only);
        move_set = &Moves::Blocks::ROOK[queen].checked_moves[rook_index];
        if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;
    }

    // Rook checked moves.
    piece = this->turn ? W_ROOK : B_ROOK;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int rook = this->piece_list[piece][i];
        if ((1ULL << rook) & this->rook_pins || (1ULL << rook) & this->bishop_pins) {
            continue;
        }
        MovesStruct* rook_move = &Moves::ROOK[Indices::ROOK[rook][
                rookIndex(this->sides[BLACK] | this->sides[WHITE], 
                (Square) rook)]];
        uint64_t rook_index = rook_move->reach & (check_rays_only | checkers_only);
        std::vector<Move>* move_set = &Moves::Blocks::ROOK[rook].checked_moves[rook_index];
        if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;
    }

    // Bishop checked moves.
    piece = this->turn ? W_BISHOP : B_BISHOP;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int bishop = this->piece_list[piece][i];
        if ((1ULL << bishop) & this->rook_pins || (1ULL << bishop) & this->bishop_pins) {
            continue;
        }
        // displayBB(checkers_and_rays);
        MovesStruct* bishop_move = &Moves::BISHOP[Indices::BISHOP[bishop][
                bishopIndex(this->sides[BLACK] | this->sides[WHITE], 
                (Square) bishop)]];
        uint64_t bishop_index = bishop_move->reach & (check_rays_only | 
                checkers_only);
        std::vector<Move>* move_set = &Moves::Blocks::BISHOP[bishop].checked_moves[bishop_index];
        if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;
    }

    // Knight checked moves.
    piece = this->turn ? W_KNIGHT : B_KNIGHT;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int knight = this->piece_list[piece][i];
        if ((1ULL << knight) & this->rook_pins || (1ULL << knight) & this->bishop_pins) {
            continue;
        }
        MovesStruct * knight_move = &Moves::KNIGHT[knight];
        uint64_t masked_reach = knight_move->reach & (check_rays_only | 
                checkers_only);
        std::vector<Move>* move_set = &knight_move->move_set[moveSetIndex(
                masked_reach, knight_move)];
        if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;
    }

    // Pawn checked moves.
    piece = this->turn ? W_PAWN : B_PAWN;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int pawn = this->piece_list[piece][i];
        if ((1ULL << pawn) & this->rook_pins || (1ULL << pawn) & this->bishop_pins) {
            continue;
        }
        
        MovesStruct* pawn_move = &Moves::PAWN[this->turn][pawn - 8];
        int advance = this->turn ? 8 : -8;
        uint64_t masked_reach = (pawn_move->reach & checkers_only) | (1ULL << 
                (pawn + advance));
        std::vector<Move>* move_set = &pawn_move->move_set[moveSetIndex(
                masked_reach, pawn_move)];
        if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;

        // Pawn single advance to block check.
        if ((1ULL << (pawn + advance)) & check_rays_only) {
            uint64_t pos = (1ULL << (pawn + advance + advance));
            MovesStruct* pawn_moves = &Moves::PAWN[this->turn][pawn - 8];
            move_set = &pawn_moves->move_set[
                    moveSetIndex(pos, pawn_moves)];
            if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;
        }

        // Pawn double advance to block check.
        advance = this->turn ? 16 : -16;
        int between = this->turn ? 8 : -8;
        if (((pawn / 8 == 1 && this->turn) || (pawn / 8 == 6 && !this->turn)) && 
                (1ULL << (pawn + advance)) & check_rays_only && !((1ULL << (
                pawn + between)) & (this->sides[BLACK] | this->sides[WHITE]))) {
            MovesStruct* pawn_moves = &Moves::DOUBLE_PUSH[pawn - (this->turn ? 8 : 40)];
            move_set = &pawn_moves->move_set[0];
            if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;
        }
    }

    // En-passant moves.
    this->getCheckedEp(checkers_only, pos_moves, moves_index);
}

/**
 * Gets a bitboard of the rays between the bishop (queen) checkers and the king
 * being checked.
 * 
 * @param game: A pointer to game struct.
 * @param moves: A pointer to precomputed moves.
 * @param square: The square of the king in check.
 * @param checkers_only: Pointer to bit board of checkers only.
 */
uint64_t Pos::getBishopCheckRays(Square square, uint64_t* checkers_only) {
    uint64_t result = 0;
    bool turn = this->turn;
    Square king_sq = this->piece_list[turn][0];
    uint64_t king_rays = this->getBishopFamily(king_sq)->reach & 
            ~this->sides[turn];
    *checkers_only |= (this->queens | this->bishops) & this->sides[!turn] & 
            king_rays;
    PieceType queen = turn ? B_QUEEN : W_QUEEN;
    PieceType bishop = turn ? B_BISHOP : W_BISHOP;

    for (int i = 0; i < this->piece_index[queen]; i++) {
        Square piece = this->piece_list[queen][i];
        if (std::abs(piece % 8 - king_sq % 8) == std::abs(piece / 8 - king_sq / 
                8)) {
            result |= (this->getBishopFamily((Square) piece)->reach & 
                    ~this->sides[!turn] & king_rays);
        }
    }

    for (int i = 0; i < this->piece_index[bishop]; i++) {
        Square piece = this->piece_list[bishop][i];
        if (std::abs(piece % 8 - king_sq % 8) == std::abs(piece / 8 - king_sq / 
                8)) {
            result |= (this->getBishopFamily((Square) piece)->reach & 
                    ~this->sides[!turn] & king_rays);
        }
    }

    return result;
}

/**
 * Gets a bitboard of the rays between the rook (queen) checkers and the king
 * being checked.
 * @param game: A pointer to game struct.
 * @param moves: A pointer to precomputed moves.
 * @param square: The square of the king in check.
 * @param checkers: Bitboard of checkers.
 * @return: Check rays of rooks and queens.
 */
uint64_t Pos::getRookCheckRays(Square square, uint64_t* checkers) {
    uint64_t result = 0;
    Square king_sq = this->piece_list[this->turn][0];
    uint64_t king_rays = this->getRookFamily(king_sq)->reach & ~this->sides[this->turn];
    *checkers |= (this->queens | this->rooks) & this->sides[1 - this->turn] & king_rays;
    PieceType queen = this->turn ? B_QUEEN : W_QUEEN;
    PieceType rook = this->turn ? B_ROOK : W_ROOK;

    for (int i = 0; i < this->piece_index[queen]; i++) {
        Square queen_sq = this->piece_list[queen][i];
        if (queen_sq % 8 == king_sq % 8 || queen_sq / 8 == king_sq / 8) {
            result |= (this->getRookFamily((Square) queen_sq)->reach & 
                    ~this->sides[1 - this->turn] & king_rays);
        }
    }

    for (int i = 0; i < this->piece_index[rook]; i++) {
        Square rook_sq = this->piece_list[rook][i];
        if (rook_sq % 8 == king_sq % 8 || rook_sq / 8 == king_sq / 8) {
            result |= (this->getRookFamily((Square) rook_sq)->reach & 
                    ~this->sides[1 - this->turn] & king_rays);
        }
    }

    return result;
}

/**
 * Returns a bitboard of all pawn checkers.
 * @param game: The game struct pointer of the current position.
 * @param square: The square of the king in check.
 * @param checkers_only: Pointer to bit board of checkers only.
 */
uint64_t Pos::getPawnCheckers(Square square, uint64_t* checkers_only) {
    uint64_t enemy_pawns = this->sides[1 - this->turn] & this->pawns;
    uint64_t result = 0;
    int rank_offset = this->turn ? 8 : -8;
    if (square % 8 != 0) {
        int pawn_sq = square + rank_offset - 1;
        if ((1ULL << pawn_sq) & enemy_pawns) {
            *checkers_only |= 1ULL << pawn_sq;
        }
    }

    if (square % 8 != 7) {
        int pawn_sq = square + rank_offset + 1;
        if ((1ULL << pawn_sq) & enemy_pawns) {
            *checkers_only |= 1ULL << pawn_sq;
        }
    }

    return result;
}

/**
 * Gets the en-passant moves for a normal move of the game.
 * @param game: A game struct pointer.
 * @param rook_pins: Bitboard of pieces pinned horiztonally or vertically.
 * @param bishop_pins: Bitboard of pieces pinned diagonally.
 * @param checkers: The bitboard of checkers.
 * @param moves: Precomputed moves struct pointer.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getCheckedEp(uint64_t checkers, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int& moves_index) {
    if (this->en_passant != NONE) {
        int rank_offset = this->turn ? -8 : 8;
        int ep = this->en_passant;
        uint64_t own_pawns = this->sides[this->turn] & this->pawns;

        if (ep % 8 != 0) { // Capture from left to block.
            int attack_sq = ep + rank_offset - 1;
            if (((1ULL << ep) & checkers) && (own_pawns & (1ULL << attack_sq)) && !((1ULL << attack_sq) &
                    this->rook_pins) && !((1ULL << attack_sq) & this->bishop_pins)) {
                if (attack_sq % 8 < ep % 8) {
                    std::vector<Move>* move_set = &Moves::EN_PASSANT[
                            attack_sq - 24].move_set[0];
                    if (move_set->size() != 0) {
                        pos_moves[(moves_index)++] = move_set;
                    }
                } else {
                    std::vector<Move>* move_set = &Moves::EN_PASSANT[
                            attack_sq - 24].move_set[1];
                    if (move_set->size() != 0) {
                        pos_moves[(moves_index)++] = move_set;
                    }
                }
            }
        }

        if (ep % 8 != 7) { // Capture from right to block.
            int attack_sq = ep + rank_offset + 1;
            if (((1ULL << ep) & checkers) && (own_pawns & 
                    (1ULL << attack_sq)) && !((1ULL << attack_sq) & 
                    this->rook_pins) && !((1ULL << attack_sq) & this->bishop_pins)) {
                if (attack_sq % 8 < ep % 8) {
                    std::vector<Move>* move_set = &Moves::EN_PASSANT[
                            attack_sq - 24].move_set[0];
                    if (move_set->size() != 0) {
                        pos_moves[(moves_index)++] = move_set;
                    }
                } else {
                    std::vector<Move>* move_set = &Moves::EN_PASSANT[
                            attack_sq - 24].move_set[1];
                    if (move_set->size() != 0) {
                        pos_moves[(moves_index)++] = move_set;
                    }
                }
            }
        }

        // Capture checker
        if (ep % 8 != 0) { // Capture checker from left
            int pawn = ep + rank_offset;
            int attack_sq = ep + rank_offset - 1;
            if (((1ULL << pawn) & checkers) && (own_pawns & (1ULL << 
                    attack_sq)) && !((1ULL << attack_sq) & this->rook_pins) && 
                    !((1ULL << attack_sq) & this->bishop_pins)) {
                if (attack_sq % 8 < ep % 8) {
                    std::vector<Move>* move_set = &Moves::EN_PASSANT[
                            attack_sq - 24].move_set[0];
                    if (move_set->size() != 0) {
                        pos_moves[(moves_index)++] = move_set;
                    }
                } else {
                    std::vector<Move>* move_set = &Moves::EN_PASSANT[
                            attack_sq - 24].move_set[1];
                    if (move_set->size() != 0) {
                        pos_moves[(moves_index)++] = move_set;
                    }
                }
            }
        }

        if (ep % 8 != 7) {
            int pawn = ep + rank_offset;
            int attack_sq = ep + rank_offset + 1;
            if (((1ULL << pawn) & checkers) && (own_pawns & (1ULL << 
                    attack_sq)) && !((1ULL << attack_sq) & this->rook_pins) && 
                    !((1ULL << attack_sq) & this->bishop_pins)) {
                if (attack_sq % 8 < ep % 8) {
                    std::vector<Move>* move_set = &Moves::EN_PASSANT[
                            attack_sq - 24].move_set[0];
                    if (move_set->size() != 0) {
                        pos_moves[(moves_index)++] = move_set;
                    }
                } else {
                    std::vector<Move>* move_set = &Moves::EN_PASSANT[
                            attack_sq - 24].move_set[1];
                    if (move_set->size() != 0) {
                        pos_moves[(moves_index)++] = move_set;
                    }
                }
            }
        }
    }
}

/**
 * Computes the initial hash of the position. Basically, just computes the current hash.
 */
void Pos::initialiseHash() {
    this->hash = 0ULL;

    // Hash the pieces
    for (int sq = A1; sq <= H8; sq++) {
        if (this->pieces[sq] != NO_PIECE) {
            this->hash ^= Hashes::PIECES[this->pieces[sq]][sq];
        }
    }

    // Hash turn
    if (this->turn == BLACK) this->hash ^= Hashes::TURN;

    // Hash the castling
    this->hash ^= Hashes::CASTLING[this->castling];

    // Hash en-passant
    if (this->en_passant != NONE) {
        this->hash ^= Hashes::EN_PASSANT[this->en_passant % 8];
    }
}

/**
 * Gets all legal moves for the player whose turn it is.
 * @param game: A game struct pointer.
 * @param computed_moves: Precomputed moves struct pointer.
 * @param enemy_attacks: 64 bit unsigned int pointer for computing king moves.
 * @param rook_pins: 64 bit unsigned int pointer for rook pins.
 * @param bishop_pins: 64 bit unsigned int pointer for bishop pins.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getNormalMoves(std::vector<Move>* pos_moves[MAX_MOVE_SETS], int& moves_index) {
    this->getKingMoves(pos_moves, moves_index);
    this->getQueenMoves(pos_moves, moves_index);
    this->getRookMoves(pos_moves, moves_index);
    this->getBishopMoves(pos_moves, moves_index);
    this->getKnightMoves(pos_moves, moves_index);
    this->getPawnMoves(pos_moves, moves_index);
    this->getCastlingMoves(pos_moves, moves_index);
    this->getEpMoves(pos_moves, moves_index);
}

/**
 * Get queen moves.
 * @param game: Pointer to game struct.
 * @param computed_moves: Pointer to precomputed moves struct.
 * @param rook_pins: Bitboard of pieces pinned by rooks.
 * @param bishop_pins: Bitboard of piecse pinned by bishops.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getQueenMoves(std::vector<Move>* pos_moves[MAX_MOVE_SETS], int& moves_index) {
    PieceType piece = this->turn ? W_QUEEN : B_QUEEN;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int queen = this->piece_list[piece][i];
        if (rook_pins & (1ULL << queen)) { // Pinned.
            this->getRookPinMoves(queen, pos_moves, moves_index);
        } else if (bishop_pins & (1ULL << queen)) { // Pinned.
            this->getBishopPinMoves(queen, pos_moves, moves_index);
        } else {
            MovesStruct* rook_like_moves = this->getRookFamily((Square) queen);
            std::vector<Move>* move_set = &rook_like_moves->move_set[
                    moveSetIndex(rook_like_moves->reach ^ this->sides[
                    this->turn], rook_like_moves)];
            if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;

            MovesStruct* bishop_like_moves = this->getBishopFamily((Square) queen);
            move_set = &bishop_like_moves->move_set[
                    moveSetIndex(bishop_like_moves->reach ^ this->sides[
                    this->turn], bishop_like_moves)];
            if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;
        }
    }
}

/**
 * Get rook or queen moves if piece is pinned horizontally or vertically.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param square: The square of the piece pinned.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getRookPinMoves(int square, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int& moves_index) {
    int king = this->piece_list[this->turn][0];
    uint64_t pos = this->sides[BLACK] | this->sides[WHITE];
    uint64_t friendly = 0;
    if (square == A1 || square == A8 || square == H1 || square == H8) {
        return;
    } else if (square / 8 == 0) {
        pos |= files[square % 8];
        friendly |= 1ULL << (square + 8);
    } else if (square / 8 == 7) {
        pos |= files[square % 8];
        friendly |= 1ULL << (square - 8);
    } else if (square % 8 == 0) {
        pos |= ranks[square / 8];
        friendly |= 1ULL << (square + 1);
    } else if (square % 8 == 7) {
        pos |= ranks[square / 8];
        friendly |= 1ULL << (square - 1);
    } else { // Middle
        if (king % 8 == square % 8) { // Vertical pin
            pos |= ranks[square / 8];
            friendly |= 1ULL << (square + 1);
            friendly |= 1ULL << (square - 1);
        } else { // Horizontal pin
            pos |= files[square % 8];
            friendly |= 1ULL << (square + 8);
            friendly |= 1ULL << (square - 8);
        }
    }
    MovesStruct* rook_moves = &Moves::ROOK[Indices::ROOK[square][rookIndex(
            pos, (Square) square)]];
    std::vector<Move>* move_set = &rook_moves->move_set[moveSetIndex(
            rook_moves->reach ^ (this->sides[this->turn] | friendly), 
            rook_moves)];
    if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;
}

/**
 * Get bishop or queen moves if piece is pinned diagonally.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param square: The square of the piece pinned.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getBishopPinMoves (int square, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int& moves_index) {
    int king = this->piece_list[this->turn][0];
    uint64_t pos = this->sides[BLACK] | this->sides[WHITE];
    uint64_t friendly = 0;
    if (square / 8 == 0 || square / 8 == 7 || square % 8 == 0 || 
            square % 8 == 7) {
        return;
    } else {
        if ((king > square && (king % 8) > (square % 8)) || (king < square && 
                (king % 8) < (square % 8))) { // [/] pin
            pos |= 1ULL << (square + 7) | 1ULL << (square - 7);
            friendly |= 1ULL << (square + 7);
            friendly |= 1ULL << (square - 7);
        } else { // [\] pin
            pos |= 1ULL << (square + 9) | 1ULL << (square - 9);
            friendly |= 1ULL << (square + 9);
            friendly |= 1ULL << (square - 9);
        }
    }
    MovesStruct* bishop_moves = &Moves::BISHOP[Indices::BISHOP[square]
            [bishopIndex(pos, (Square) square)]];
    std::vector<Move>* move_set = &bishop_moves->move_set[moveSetIndex(bishop_moves->reach ^ (this->sides[this->turn] |
            friendly), bishop_moves)];
    if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;
}

/**
 * Get rook moves.
 * @param game: Pointer to game struct.
 * @param computed_moves: Pointer to precomputed moves struct.
 * @param rook_pins: Bitboard of pieces pinned by rooks.
 * @param bishop_pins: Bitboard of piecse pinned by bishops.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getRookMoves(std::vector<Move>* pos_moves[MAX_MOVE_SETS], int& moves_index) {
    PieceType piece = this->turn ? W_ROOK : B_ROOK;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int rook = this->piece_list[piece][i];
        if (rook_pins & (1ULL << rook)) { // Pinned.
            this->getRookPinMoves(rook, pos_moves, moves_index);
        } else if (bishop_pins & (1ULL << rook)) {
            continue;
        } else {
            MovesStruct* rook_like_moves = this->getRookFamily((Square) rook);
            std::vector<Move>* move_set = &rook_like_moves->move_set[
                    moveSetIndex(rook_like_moves->reach ^ this->
                    sides[this->turn], rook_like_moves)];
            if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;
        }
    }
}

/**
 * Get bishop moves.
 * @param game: Pointer to game struct.
 * @param computed_moves: Pointer to precomputed moves struct.
 * @param rook_pins: Bitboard of pieces pinned by rooks.
 * @param bishop_pins: Bitboard of piecse pinned by bishops.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getBishopMoves(std::vector<Move>* pos_moves[MAX_MOVE_SETS], int& moves_index) {
    PieceType piece = this->turn ? W_BISHOP : B_BISHOP;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int bishop = this->piece_list[piece][i];
        if (rook_pins & (1ULL << bishop)) { // Pinned.
            continue;
        } else if (bishop_pins & (1ULL << bishop)) { // Pinned.
            this->getBishopPinMoves(bishop, pos_moves, moves_index);
        } else {
            MovesStruct* bishop_like_moves = this->getBishopFamily((Square) bishop);
            std::vector<Move>* move_set = &bishop_like_moves->move_set[
                    moveSetIndex(bishop_like_moves->reach ^ this->sides[
                    this->turn], bishop_like_moves)];
            if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;
        }
    }
}

/**
 * Get knight moves.
 * @param game: Pointer to game struct.
 * @param computed_moves: Pointer to precomputed moves struct.
 * @param rook_pins: Bitboard of pieces pinned by rooks.
 * @param bishop_pins: Bitboard of piecse pinned by bishops.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getKnightMoves(std::vector<Move>* pos_moves[MAX_MOVE_SETS], int& moves_index) {
    PieceType piece = this->turn ? W_KNIGHT : B_KNIGHT;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int knight = this->piece_list[piece][i];
        if (rook_pins & (1ULL << knight) || bishop_pins & (1ULL << knight)) { // Pinned.
            continue;
        } else {
            MovesStruct* knight_moves = &Moves::KNIGHT[knight];
            std::vector<Move>* move_set = &knight_moves->move_set[moveSetIndex(knight_moves->reach ^ this->sides[
                    this->turn], knight_moves)];
            if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;
        }
    }
}

/**
 * Get pawn moves.
 * @param game: Pointer to game struct.
 * @param computed_moves: Pointer to precomputed moves struct.
 * @param rook_pins: Bitboard of pieces pinned by rooks.
 * @param bishop_pins: Bitboard of piecse pinned by bishops.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getPawnMoves(std::vector<Move>* pos_moves[MAX_MOVE_SETS], int& moves_index) {
    Square king = this->piece_list[this->turn][0];
    PieceType piece = this->turn ? W_PAWN : B_PAWN;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int pawn = this->piece_list[piece][i];
        if (rook_pins & (1ULL << pawn)) { // Pinned
            if (pawn % 8 == king % 8) {
                MovesStruct* pawn_moves = &Moves::PAWN[this->turn][
                        pawn - 8];
                std::vector<Move>* move_set = &pawn_moves->move_set[
                        moveSetIndex((this->sides[WHITE] | 
                        this->sides[BLACK]) & files[pawn % 8], pawn_moves)];
                if (move_set->size() != 0) {
                    pos_moves[(moves_index)++] = move_set;
                }
            } else continue;
        } else if (bishop_pins & (1ULL << pawn)) { // Pinned
            int ray = pawn > king ? (pawn % 8 < king % 8 ? 7 : 9) : (pawn % 8 < 
                    king % 8 ? -9 : -7) ;
            if ((this->turn == BLACK && ray > 0) || (this->turn == WHITE && 
                    ray < 0)) {
                continue;
            }
            int square = (Square) pawn + ray;
            uint64_t enemies = this->sides[1 - this->turn];

            if (enemies & (1ULL << square)) {
                int advance = this->turn ? 8 : -8;
                uint64_t pos = (1ULL << (pawn + advance)) | (1ULL << (pawn + 
                        ray));
                MovesStruct* pawn_moves = &Moves::PAWN[this->turn][
                        pawn - 8];
                std::vector<Move>* move_set = &pawn_moves->move_set[
                        moveSetIndex(pos, pawn_moves)];
                if (move_set->size() != 0) {
                    pos_moves[(moves_index)++] = move_set;
                }
            }
        } else {
            MovesStruct* pawn_moves = &Moves::PAWN[this->turn][pawn - 
                    8];
            std::vector<Move>* move_set = &pawn_moves->move_set[
                    moveSetIndex(this->pawnMoveArgs((Square) pawn), 
                    pawn_moves)];
            if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;
        }
    }
}

/**
 * Get the arguments for the pawn index function for pawns.
 * @param game: Pointer to game struct.
 * @param square: Square of pawn.
 */
uint64_t Pos::pawnMoveArgs(Square square) {
    bool turn = this->turn;
    return (this->sides[!turn] | (this->sides[turn] & files[square % 8]));
}

/**
 * Gets the castling moves.
 * 
 * @param game: A game struct pointer.
 * @param enemy_attacks: 64 bit unsigned int pointer for computing king moves.
 * @param moves: Precomputed moves struct pointer.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getCastlingMoves(std::vector<Move>* pos_moves[MAX_MOVE_SETS], int& moves_index) {
    uint64_t sides = this->sides[WHITE] | this->sides[BLACK];
    if (this->turn) {
        if (this->castling & (1 << WKSC)) {
            if (!((1ULL << F1) & enemy_attacks) && !((1ULL << G1) & 
                enemy_attacks) && !((1ULL << F1) & sides) && !((1ULL << G1) & 
                sides)) {
                std::vector<Move>* move_set = &Moves::CASTLING[WKSC].
                        move_set[0];
                if (move_set->size() != 0) {
                    pos_moves[(moves_index)++] = move_set;
                }
            }
        }

        if (this->castling & (1 << WQSC)) {
            // CmdLineArgs args;
            if (!((1ULL << D1) & enemy_attacks) && !((1ULL << C1) & 
                enemy_attacks) && !((1ULL << D1) & sides) && !((1ULL << C1) & 
                sides) && !((1ULL << B1) & sides)) {
                std::vector<Move>* move_set = &Moves::CASTLING[WQSC].
                        move_set[0];
                if (move_set->size() != 0) {
                    pos_moves[(moves_index)++] = move_set;
                }
            }
        }
    } else {
        if (this->castling & (1 << BKSC)) {
            if (!((1ULL << F8) & enemy_attacks) && !((1ULL << G8) & 
                enemy_attacks) && !((1ULL << F8) & sides) && !((1ULL << G8) & 
                sides)) {
                std::vector<Move>* move_set = &Moves::CASTLING[BKSC].
                        move_set[0];
                if (move_set->size() != 0) {
                    pos_moves[(moves_index)++] = move_set;
                }
            }
        }

        if (this->castling & (1 << BQSC)) {
            if (!((1ULL << D8) & enemy_attacks) && !((1ULL << C8) & 
                enemy_attacks) && !((1ULL << D8) & sides) && !((1ULL << C8) & 
                sides) && !((1ULL << B8) & sides)) {
                std::vector<Move>* move_set = &Moves::CASTLING[BQSC].
                        move_set[0];
                if (move_set->size() != 0) {
                    pos_moves[(moves_index)++] = move_set;
                }
            }
        }
    }
}

/**
 * Gets the en-passant moves for a normal move of the game.
 * @param game: A game struct pointer.
 * @param rook_pins: Bitboard of pieces pinned horiztonally or vertically.
 * @param bishop_pins: Bitboard of pieces pinned diagonally.
 * @param moves: Precomputed moves struct pointer.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::getEpMoves(std::vector<Move>* pos_moves[MAX_MOVE_SETS], int& moves_index) {
    if (this->en_passant != NONE) {
        bool turn = this->turn;
        int rank_offset = turn ? -8 : 8;
        int ep = this->en_passant;
        uint64_t own_pawns = this->sides[turn] & this->pawns;
        int king_sq = this->piece_list[turn][0];
        int captured_pawn = ep + rank_offset;

        if (ep % 8 != 0) { // Capture to right.
            int attacker_sq = ep + rank_offset - 1;
            if (own_pawns & (1ULL << attacker_sq) && 
                    !((1ULL << attacker_sq) & rook_pins)) {
                if (captured_pawn / 8 == king_sq / 8) {
                    this->horizontalPinEp(king_sq, turn, attacker_sq, captured_pawn, ep, pos_moves, moves_index);
                } else if ((1ULL << attacker_sq) & bishop_pins) {
                    this->diagonalPinEp(king_sq, turn, attacker_sq, captured_pawn, ep, pos_moves, moves_index);
                } else {
                    if (attacker_sq % 8 < ep % 8) {
                        std::vector<Move>* move_set = &Moves::EN_PASSANT[attacker_sq - 24].move_set[0];
                        if (move_set->size() != 0) {
                            pos_moves[(moves_index)++] = move_set;
                        }
                    } else {
                        std::vector<Move>* move_set = &Moves::EN_PASSANT[attacker_sq - 24].move_set[1];
                        if (move_set->size() != 0) {
                            pos_moves[(moves_index)++] = move_set;
                        }
                    }
                }
            }
        }
        
        if (ep % 8 != 7) { // Capture to left.
            int attacker_sq = ep + rank_offset + 1;
            if (own_pawns & (1ULL << attacker_sq) && 
                    !((1ULL << attacker_sq) & rook_pins)) {
                if (captured_pawn / 8 == king_sq / 8) {
                    this->horizontalPinEp(king_sq, turn, attacker_sq, 
                            captured_pawn, ep, pos_moves, moves_index);
                } else if ((1ULL << attacker_sq) & bishop_pins) {
                    this->diagonalPinEp(king_sq, turn, attacker_sq, 
                            captured_pawn, ep, pos_moves, moves_index);
                } else {
                    if (attacker_sq % 8 < ep % 8) {
                        std::vector<Move>* move_set = &Moves::EN_PASSANT[attacker_sq - 24].move_set[0];
                        if (move_set->size() != 0) {
                            pos_moves[(moves_index)++] = move_set;
                        }
                    } else {
                        std::vector<Move>* move_set = &Moves::EN_PASSANT[attacker_sq - 24].move_set[1];
                        if (move_set->size() != 0) {
                            pos_moves[(moves_index)++] = move_set;
                        }
                    }
                }
            }
        }
    }
}

/**
 * Gets en-passant moves in special cases. Such a case example is 
 * k7/8/8/1K1pP2q/8/8/8/8 w - d6 0 1
 * 
 * @param game: A game struct pointer.
 * @param king: The square that the king is on.
 * @param turn: The turn of the player.
 * @param attacker_sq: Square of the pawn that can potentially capture.
 * @param captured_pawn: Square of the pawn that can potentially be captured.
 * @param moves: Precomputed moves struct pointer.
 * @param ep: En-passant square.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::horizontalPinEp(int king, bool turn, int attacker_sq, int captured_pawn, int ep, 
        std::vector<Move>* pos_moves[MAX_MOVE_SETS], int& moves_index) {
    int rank = (king / 8) * 8, rank_end = rank + 7;
    PieceType e_rook = turn ? B_ROOK : W_ROOK;
    PieceType e_queen = turn ? B_QUEEN : W_QUEEN;
    int e_rook_sq = -1, e_queen_sq = -1;
    uint64_t pawns = 1ULL << attacker_sq | 1ULL << captured_pawn;
    uint64_t pieces = this->sides[WHITE] | this->sides[BLACK];

    // Find potential enemy rook and queen squares on the same rank.
    for (int i = rank; i <= rank_end; i++) {
        if (this->pieces[i] == e_rook) e_rook_sq = i;
        else if (this->pieces[i] == e_queen) e_queen_sq = i;
    }

    uint64_t king_reach = Moves::ROOK[Indices::ROOK[king][rookIndex(
            pieces ^ pawns, (Square) king)]].reach;
    uint64_t pin_ray = 0;

    // Ray between rook and king without pawns of interest.
    uint64_t rook_reach = 0;
    if (e_rook_sq != -1) {
        rook_reach = Moves::ROOK[Indices::ROOK[e_rook_sq][rookIndex(
                pieces ^ pawns, (Square) e_rook_sq)]].reach;
        pin_ray |= (king_reach & rook_reach);
    }

    // Ray between queen and king without pawns of interest.
    uint64_t queen_reach = 0;
    if (e_queen_sq != -1) {
        queen_reach = Moves::ROOK[Indices::ROOK[e_queen_sq][rookIndex(
                pieces ^ pawns, (Square) e_queen_sq)]].reach;
        pin_ray |= (king_reach & queen_reach);
    }

    if (!(pawns & pin_ray)) {
        if (attacker_sq % 8 < ep % 8) {
            std::vector<Move>* move_set = &Moves::EN_PASSANT[
                    attacker_sq - 24].move_set[0];
            if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;
        } else {
            std::vector<Move>* move_set = &Moves::EN_PASSANT[
                    attacker_sq - 24].move_set[1];
            if (move_set->size() != 0) pos_moves[(moves_index)++] = move_set;
        }
    }
}

/**
 * Gets en-passant moves involving diagonal pins. Such a case example is
 * k7/2q5/8/3pP3/8/6K1/8/8 w - d6 0 1 or
 * k7/6q1/8/3pP3/8/2K5/8/8 w - d6 0 1
 * 
 * @param game: A game struct pointer.
 * @param king: The square that the king is on.
 * @param turn: The turn of the player.
 * @param attacker_sq: Square of the pawn that can potentially capture.
 * @param captured_pawn: Square of the pawn that can potentially be captured.
 * @param moves: Precomputed moves struct pointer.
 * @param ep: En-passant square.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 */
void Pos::diagonalPinEp(int king, bool turn, int attacker_sq, 
        int captured_pawn, int ep, std::vector<Move>* 
        pos_moves[MAX_MOVE_SETS], int& moves_index) {
    if (ep > attacker_sq && ep % 8 < attacker_sq % 8) { // Upper left.
        if ((king > attacker_sq && king % 8 < attacker_sq % 8) ||
                (king < attacker_sq && king % 8 > attacker_sq % 8)) {
            if (attacker_sq % 8 < ep % 8) {
                std::vector<Move>* move_set = &Moves::EN_PASSANT[
                        attacker_sq - 24].move_set[0];
                if (move_set->size() != 0) {
                    pos_moves[(moves_index)++] = move_set;
                }
            } else {
                std::vector<Move>* move_set = &Moves::EN_PASSANT[
                        attacker_sq - 24].move_set[1];
                if (move_set->size() != 0) {
                    pos_moves[(moves_index)++] = move_set;
                }
            }
        }
    } else if (ep > attacker_sq && ep % 8 > attacker_sq % 8) { // Upper right.
        if ((king > attacker_sq && king % 8 > attacker_sq % 8) ||
                (king < attacker_sq && king % 8 < attacker_sq % 8)) {
            if (attacker_sq % 8 < ep % 8) {
                std::vector<Move>* move_set = &Moves::EN_PASSANT[
                        attacker_sq - 24].move_set[0];
                if (move_set->size() != 0) {
                    pos_moves[(moves_index)++] = move_set;
                }
            } else {
                std::vector<Move>* move_set = &Moves::EN_PASSANT[
                        attacker_sq - 24].move_set[1];
                if (move_set->size() != 0) {
                    pos_moves[(moves_index)++] = move_set;
                }
            }
        }
    } else if (ep < attacker_sq && ep % 8 < attacker_sq % 8) { // Lower left.
        if ((king < attacker_sq && king % 8 < attacker_sq % 8) ||
                (king > attacker_sq && king % 8 > attacker_sq % 8)) {
            if (attacker_sq % 8 < ep % 8) {
                std::vector<Move>* move_set = &Moves::EN_PASSANT[
                        attacker_sq - 24].move_set[0];
                if (move_set->size() != 0) {
                    pos_moves[(moves_index)++] = move_set;
                }
            } else {
                std::vector<Move>* move_set = &Moves::EN_PASSANT[
                        attacker_sq - 24].move_set[1];
                if (move_set->size() != 0) {
                    pos_moves[(moves_index)++] = move_set;
                }
            }
        }
    } else { // Lower right.
        if ((king < attacker_sq && king % 8 > attacker_sq % 8) ||
                (king > attacker_sq && king % 8 < attacker_sq % 8)) {
            if (attacker_sq % 8 < ep % 8) {
                std::vector<Move>* move_set = &Moves::EN_PASSANT[
                        attacker_sq - 24].move_set[0];
                if (move_set->size() != 0) {
                    pos_moves[(moves_index)++] = move_set;
                }
            } else {
                std::vector<Move>* move_set = &Moves::EN_PASSANT[
                        attacker_sq - 24].move_set[1];
                if (move_set->size() != 0) {
                    pos_moves[(moves_index)++] = move_set;
                }
            }
        }
    }
}

/**
 * Gets a bitboard of knight checkers.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param square: The square of the king in check.
 * @param checkers_only: A pointer to a bit board of checkers only.
 * @return: Bitboard of knight checkers.
 */
uint64_t Pos::getKnightCheckers(Square square, uint64_t* checkers_only) {
    uint64_t result = 0;
    *checkers_only |= Moves::KNIGHT[square].reach & this->sides[1 - this->turn] & this->knights;
    return result;
}

/**
 * Checks if king of current player is in double-check.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @return: True if in double-check, else false.
 */
bool Pos::isDoubleChecked() {
    bool turn = this->turn;
    Square king = this->piece_list[turn][0];
    uint64_t rook_attacks = this->getRookFamily(king)->reach;
    uint64_t bishop_attacks = this->getBishopFamily(king)->reach;

    uint64_t attackers = (rook_attacks & this->rooks & this->sides[!turn]) | 
            (bishop_attacks & this->bishops & this->sides[!turn]) |
            (rook_attacks & this->queens & this->sides[!turn]) |
            (bishop_attacks & this->queens & this->sides[!turn]);
    
    attackers |= Moves::KNIGHT[king].reach & this->knights & this->
            sides[!turn];

    uint64_t enemy_pawns = this->sides[!turn] & this->pawns;
    int rank_offset = turn ? 8 : -8;
    if (king % 8 != 0) {
        int pawn_sq = king + rank_offset - 1;
        attackers |= (1ULL << pawn_sq) & enemy_pawns;
    }

    if (king % 8 != 7) {
        int pawn_sq = king + rank_offset + 1;
        attackers |= (1ULL << pawn_sq) & enemy_pawns;
    }

    return (attackers & (attackers - 1)) != 0;
}

/**
 * Finds and returns a pointer to a rook move family.
 * @param game: A pointer to a game struct representing the state of the game.
 * @param moves: A struct of the precomputed moves.
 * @param square: The square on which the rook is on.
 * @return Pointer to moves struct.
 */
MovesStruct* Pos::getRookFamily(Square square) {
    return &Moves::ROOK[Indices::ROOK[square][rookIndex(this->sides[BLACK] | this->sides[WHITE], square)]];
}

/**
 * Finds and returns a pointer to a bishop move family.
 * 
 * @param game: A pointer to a game struct representing the state of the game.
 * @param moves: A struct of the precomputed moves.
 * @param square: The square on which the bishop is on.
 */
MovesStruct* Pos::getBishopFamily(Square square) {
    return &Moves::BISHOP[Indices::BISHOP[square][bishopIndex(this->sides[BLACK] | this->sides[WHITE], square)]];
}

/**
 * Displays the bitboard.
 * @param game: A pointer to the game struct.
 * @param args: The command line arguments.
 */
void Pos::display() const {
    bool light_mode = true;
    std::cout << '\n';
    
    // Print the pieces
    for (int rank = 7; rank >= 0; rank--) {
        std::string rank_string = "";
        rank_string += (rank - 7) + '8';
        rank_string += " ";
        for (int file = 0; file < 8; file++) {
            int square = 8 * rank + file;
            PieceType piece = this->pieces[square];
            if (piece == W_KING || piece == B_KING) {
                if (this->sides[WHITE] & (1ULL << square)) {
                    rank_string += (light_mode ? "\u265A " : "\u2654 ");
                } else {
                    rank_string += (light_mode ? "\u2654 " : "\u265A ");
                }
            } else if (piece == W_QUEEN || piece == B_QUEEN) {
                if (this->sides[WHITE] & (1ULL << square)) {
                    rank_string += (light_mode ? "\u265B " : "\u2655 ");
                } else {
                    rank_string += (light_mode ? "\u2655 " : "\u265B ");
                }
            } else if (piece == W_ROOK || piece == B_ROOK) {
                if (this->sides[WHITE] & (1ULL << square)) {
                    rank_string += (light_mode ? "\u265C " : "\u2656 ");
                } else {
                    rank_string += (light_mode ? "\u2656 " : "\u265C ");
                }
            } else if (piece == W_BISHOP || piece == B_BISHOP) {
                if (this->sides[WHITE] & (1ULL << square)) {
                    rank_string += (light_mode ? "\u265D " : "\u2657 ");
                } else {
                    rank_string += (light_mode ? "\u2657 " : "\u265D ");
                }
            } else if (piece == W_KNIGHT || piece == B_KNIGHT) {
                if (this->sides[WHITE] & (1ULL << square)) {
                    rank_string += (light_mode ? "\u265E " : "\u2658 ");
                } else {
                    rank_string += (light_mode ? "\u2658 " : "\u265E ");
                }
            } else if (piece == W_PAWN || piece == B_PAWN) {
                if (this->sides[WHITE] & (1ULL << square)) {
                    rank_string += (light_mode ? "\u265F " : "\u2659 ");
                } else {
                    rank_string += (light_mode ? "\u2659 " : "\u265F ");
                }
            } else {
                rank_string += "  ";
            }
        }
        std::cout << rank_string << '\n';
    }
    std::cout << "  a b c d e f g h\n";

    // Print turn
    if (this->turn) {
        std::cout << "Turn: white\n";
    } else {
        std::cout << "Turn: black\n";
    }

    // Print castling
    std::string castling = "Castling: ";
    if (this->castling & (1 << WKSC)) castling += "K";
    if (this->castling & (1 << WQSC)) castling += "Q";
    if (this->castling & (1 << BKSC)) castling += "k";
    if (this->castling & (1 << BQSC)) castling += "q";
    if (!(this->castling & (1 << WKSC)) && !(this->castling & (1 << WQSC)) && 
            !(this->castling & (1 << BKSC)) && 
            !(this->castling & (1 << BQSC))) {
        castling += "-";
    }
    std::cout << castling << '\n';

    // Print en-passant
    std::string ep = "En-passant: ";
    if (this->en_passant == NONE) {
        ep += "-\n";
    } else {
        ep += squareName[this->en_passant] + "\n";
    }
    std::cout << ep;

    // Clock cycles
    std::cout << "Halfmove: " << this->halfmove << '\n';
    std::cout << "Fullmove: " << this->fullmove << '\n';
}

/**
 * Computes and returns the hash of the current position.
 * @param game: Pointer to game struct.
 */
uint64_t getPosHash(Pos* game) {
    uint64_t hash = 0ULL;
    // if (game->ply == 0) { // Start of game, loop through all pieces.
    //     for (int sq = A1; sq <= H8; sq++) {
    //         if (game->pieces[sq] == W_KING) {
    //             hash ^= ZOBRIST_WKING[sq];
    //         } else if (game->pieces[sq] == W_QUEEN) {
    //             hash ^= ZOBRIST_WQUEEN[sq];
    //         } else if (game->pieces[sq] == W_ROOK) {
    //             hash ^= ZOBRIST_WROOK[sq];
    //         } else if (game->pieces[sq] == W_BISHOP) {
    //             hash ^= ZOBRIST_WBISHOP[sq];
    //         } else if (game->pieces[sq] == W_KNIGHT) {
    //             hash ^= ZOBRIST_WKNIGHT[sq];
    //         } else if (game->pieces[sq] == W_PAWN) {
    //             hash ^= ZOBRIST_WPAWN[sq];
    //         } else if (game->pieces[sq] == B_KING) {
    //             hash ^= ZOBRIST_BKING[sq];
    //         } else if (game->pieces[sq] == B_QUEEN) {
    //             hash ^= ZOBRIST_BQUEEN[sq];
    //         } else if (game->pieces[sq] == B_ROOK) {
    //             hash ^= ZOBRIST_BROOK[sq];
    //         } else if (game->pieces[sq] == B_BISHOP) {
    //             hash ^= ZOBRIST_BBISHOP[sq];
    //         } else if (game->pieces[sq] == B_KNIGHT) {
    //             hash ^= ZOBRIST_BKNIGHT[sq];
    //         } else if (game->pieces[sq] == B_PAWN) {
    //             hash ^= ZOBRIST_BPAWN[sq];
    //         }
    //     }

    //     hash ^= ZOBRIST_CASTLING[game->castling];
    //     // if (!game->turn) hash ^= ZOBRIST_BLACK;
    //     if (game->en_passant != NONE) {
    //         hash ^= ZOBRIST_EP[game->en_passant % 8];
    //     }
    // } else {

    // }

    return hash;
}

/**
 * Checks if a three fold repetition occured. If so, return true, else false.
 * Also performs the hash updates for the history.
 * @param game: Pointer to game struct.
 */
bool Pos::isThreeFoldRep() {
    auto val = this->hashes.find(this->hash);
    if (val != this->hashes.end() && val->second >= 3) {
        return true;
    }
    return false;
}

/**
 * Checks if end of game has been reached.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves.
 * @param enemy_attacks: Bitboard of squares attacked by enemy pieces.
 * @param moves_index: Pointer to number of vectors in pos_moves.
 * @return: The appropriate ExitCode.
 */
ExitCode Pos::isEOG(int move_index) {
    if (this->isThreeFoldRep()) return THREE_FOLD_REPETITION;

    if (this->halfmove == 100) return FIFTY_MOVES_RULE;

    if (this->insufficientMaterial()) return INSUFFICIENT_MATERIAL;

    if (move_index == 0 && !(this->isChecked())) return STALEMATE;

    if (move_index == 0 && this->isChecked()) {
        if (this->turn) return BLACK_WINS;
        return WHITE_WINS;
    }

    return NORMAL_PLY;
}

/**
 * Retrives all legal moves of the current position.
 * 
 * @param game: Pointer to Pos struct of the current position.
 * @param moves: Pointer to the precomputed moves struct.
 * @param enemy_attacks: Pointer to bitboard of all squares attacked by the 
 *  enemy.
 * @param pos_moves: Array of vectors pointers of 16 bit unsigned int moves.
 * @return: The number of move sets.
 */
void Pos::getMoves(int& moves_index, std::vector<Move>* pos_moves[MAX_MOVE_SETS]) {
    // The pinning rays
    this->rook_pins = 0ULL;
    this->bishop_pins = 0ULL;
    this-> kEnemy_attacks = 0ULL;
    this->getEnemyAttacks();

    // Move retrieval for the 3 cases.
    moves_index = 0;
    if (this->isDoubleChecked()) {
        this->getKingMoves(pos_moves, moves_index);
    } else if (this->isChecked()) {
        this->getCheckedMoves(pos_moves, moves_index);
    } else {
        this->getNormalMoves(pos_moves, moves_index);
    }
}

/**
 * Gets the squares attacked by an enemy piece.
 * 
 * @param game: A game struct pointer.
 * @param moves: Precomputed moves struct pointer.
 * @param enemy_attacks: 64 bit unsigned int pointer for computing king moves.
 * @param rook_pins: 64 bit unsigned int pointer for rook pins.
 * @param bishop_pins: 64 bit unsigned int pointer for bishop pins.
 * @param kEnemy_attacks: 64 bit unsigned int pointer with value 0.
 */
void Pos::getEnemyAttacks() {
    this->enemy_attacks = 0ULL;
    bool turn = this->turn;
    Square king_sq = this->piece_list[turn][0];
    uint64_t pieces = this->sides[WHITE] | this->sides[BLACK];
    uint64_t masked_king_bb = (this->sides[WHITE] | this->sides[BLACK]) ^ 
            (1ULL << king_sq);

    // Pawn attacks.
    PieceType piece = turn ? B_PAWN : W_PAWN;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        this->enemy_attacks |= Moves::PAWN[!turn][this->piece_list[piece][i] - 8].reach;
        this->kEnemy_attacks |= this->enemy_attacks;
    }

    // Knight attacks.
    piece = turn ? B_KNIGHT : W_KNIGHT;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        this->enemy_attacks |= Moves::KNIGHT[this->piece_list[piece][i]].reach;
        this->kEnemy_attacks |= this->enemy_attacks;
    }

    // Bishop attacks.
    uint64_t king_bishop_rays = Moves::BISHOP[Indices::BISHOP[
            king_sq][bishopIndex(pieces ^ (1ULL << king_sq), king_sq)]].reach;
    piece = turn ? B_BISHOP : W_BISHOP;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int square = this->piece_list[piece][i];
        this->enemy_attacks |= Moves::BISHOP[Indices::BISHOP[square][bishopIndex(pieces ^ (1ULL << 
                this->piece_list[turn][0]), (Square) square)]].reach;
        this->kEnemy_attacks |= Moves::BISHOP[Indices::BISHOP[square][
                bishopIndex(masked_king_bb, (Square)square)]].reach;
        
        if (std::abs(square % 8 - king_sq % 8) == std::abs(((square / 8) % 
                8) - ((king_sq / 8) % 8))) {
            // Attacks.
            uint64_t attacks = Moves::BISHOP[Indices::BISHOP[square][
                    bishopIndex(pieces ^ (1ULL << square), (Square) square)]].
                    reach;
            this->bishop_pins |= king_bishop_rays & attacks;
        }
    }

    // Rook attacks.
    uint64_t king_rook_rays = Moves::ROOK[Indices::ROOK[king_sq][
            rookIndex(pieces ^ (1ULL << king_sq), king_sq)]].reach;
    piece = turn ? B_ROOK : W_ROOK;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int square = this->piece_list[piece][i];
        this->enemy_attacks |= Moves::ROOK[Indices::ROOK[square]
                [rookIndex(pieces ^ (1ULL << this->piece_list[turn][0]), 
                (Square) square)]].reach;
        this->kEnemy_attacks |= Moves::ROOK[Indices::ROOK[square][
                rookIndex(masked_king_bb, (Square)square)]].reach;
        if (king_sq / 8 == square / 8 || king_sq % 8 == square % 8) {
            // Attacks.
            uint64_t attacks = Moves::ROOK[Indices::ROOK[square][
                    rookIndex(pieces ^ (1ULL << square), (Square) square)]].
                    reach;
            this->rook_pins |= king_rook_rays & attacks;
        }
    }

    // Queen attacks.
    piece = turn ? B_QUEEN : W_QUEEN;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        int square = this->piece_list[piece][i];
        this->enemy_attacks |= Moves::BISHOP[Indices::BISHOP[square]
                [bishopIndex(pieces ^ (1ULL << this->piece_list[turn][0]), 
                (Square) square)]].reach;
        this->enemy_attacks |= Moves::ROOK[Indices::ROOK[square]
                [rookIndex(pieces ^ (1ULL << this->piece_list[turn][0]), 
                (Square) square)]].reach;
        this->kEnemy_attacks |= Moves::BISHOP[Indices::BISHOP[square][
                bishopIndex(masked_king_bb, (Square)square)]].reach;
        this->kEnemy_attacks |= Moves::ROOK[Indices::ROOK[square][
                rookIndex(masked_king_bb, (Square)square)]].reach;
        
        if (std::abs(square % 8 - king_sq % 8) == std::abs(((square / 8) % 
                8) - ((king_sq / 8) % 8))) {
            // Attacks.
            uint64_t attacks = Moves::BISHOP[Indices::BISHOP[square][
                    bishopIndex(pieces ^ (1ULL << square), (Square) square)]].
                    reach;
            this->bishop_pins |= king_bishop_rays & attacks;
        }

        if (king_sq / 8 == square / 8 || king_sq % 8 == square % 8) {
            // Attacks.
            uint64_t attacks = Moves::ROOK[Indices::ROOK[square][
                    rookIndex(pieces ^ (1ULL << square), (Square) square)]].
                    reach;
            this->rook_pins |= king_rook_rays & attacks;
        }
    }

    // King attacks.
    this->enemy_attacks |= Moves::KING[this->piece_list[!turn][0]].reach;
    this->kEnemy_attacks |= this->enemy_attacks;
}

/**
 * Find the piece in their associated piece list and removes it.
 * @param game: Pointer to game struct.
 * @param piece: The piece type to find and remove.
 * @param square: The square of the piece type to find and remove.
 */
void Pos::findAndRemovePiece(PieceType piece, Square square) {
    int taken_index = -1;
    for (int i = 0; i < this->piece_index[piece]; i++) {
        if (this->piece_list[piece][i] == square) {
            taken_index = i;
            break;
        }
    }

    this->piece_index[piece]--;
    this->piece_list[piece][taken_index] = this->piece_list[piece][this->
            piece_index[piece]];
    if (piece == W_BISHOP) {
        if (isDark(square)) {
            this->wdsb_cnt--;
        } else {
            this->wlsb_cnt--;
        }
    } else if (piece == B_BISHOP) {
        if (isDark(square)) {
            this->bdsb_cnt--;
        } else {
            this->blsb_cnt--;
        }
    } else if (piece == W_KNIGHT || piece == B_KNIGHT) {
        this->knight_cnt--;
    }
    this->piece_cnt--;
}

/**
 * Adds a piece to the end of the appropriate piece list.
 * @param game: Pointer to game struct.
 * @param piece: The piece type to add.
 * @param square: The square to add for piece.
 */
void Pos::addPiece(PieceType piece, Square square) {
    this->piece_list[piece][this->piece_index[piece]] = square;
    this->piece_index[piece]++;
    if (piece == W_BISHOP) {
        if (isDark(square)) {
            this->wdsb_cnt++;
        } else {
            this->wlsb_cnt++;
        }
    } else if (piece == B_BISHOP) {
        if (isDark(square)) {
            this->bdsb_cnt++;
        } else {
            this->blsb_cnt++;
        }
    } else if (piece == W_KNIGHT || piece == B_KNIGHT) {
        this->knight_cnt++;
    }
    this->piece_cnt++;
}

/**
 * Process the removal of a piece just captured, if any.
 * @param game: Pointer to game struct.
 */
void Pos::removePiece() {
    PieceType captured = this->piece_captured;
    if (captured == NO_PIECE) return;
    int end = (this->last_move >> 6) & 0b111111;
    if (captured == W_QUEEN || captured == B_QUEEN) {
        this->queens &= ~(1ULL << end);
        this->findAndRemovePiece(captured, (Square) end);
    } else if (captured == W_ROOK || captured == B_ROOK) {
        this->rooks &= ~(1ULL << end);
        this->findAndRemovePiece(captured, (Square) end);
    } else if (captured == W_BISHOP || captured == B_BISHOP) {
        this->bishops &= ~(1ULL << end);
        this->findAndRemovePiece(captured, (Square) end);
    } else if (captured == W_KNIGHT || captured == B_KNIGHT) {
        this->knights &= ~(1ULL << end);
        this->findAndRemovePiece(captured, (Square) end);
    } else if (captured == W_PAWN || captured == B_PAWN) {
        this->pawns &= ~(1ULL << end);
        this->findAndRemovePiece(captured, (Square) end);
    }
}

/**
 * Process castling.
 * @param game: Pointer to game struct.
 */
void Pos::handleCastle() {
    int start, end;
    int last_end = (this->last_move >> 6) & 0b111111;
    PieceType rook;

    if (last_end  == G1) {
        start = H1;
        end = F1;
        rook = W_ROOK;
    } else if (last_end == C1) {
        start = A1;
        end = D1;
        rook = W_ROOK;
    } else if (last_end == G8) {
        start = H8;
        end = F8;
        rook = B_ROOK;
    } else {
        start = A8;
        end = D8;
        rook = B_ROOK;
    }

    this->sides[this->turn] &= ~(1ULL << start);
    this->sides[this->turn] |= 1ULL << end;
    this->pieces[start] = NO_PIECE;
    this->hash ^= Hashes::PIECES[rook][start];
    this->pieces[end] = rook;
    this->hash ^= Hashes::PIECES[rook][end];

    this->findAndRemovePiece(rook, (Square) start);
    this->addPiece(rook, (Square) end);
    this->rooks &= ~(1ULL << start);
    this->rooks |= 1ULL << end;
}

/**
 * Make the king move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Pos::makeKingMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    int move_type = move & (0b11 << 12);
    this->piece_list[this->turn][0] = (Square) end;

    // Remove castling rights
    if (this->turn) {
        this->castling &= ~(1 << WKSC | 1 << WQSC);
    } else {
        this->castling &= ~(1 << BKSC | 1 << BQSC);
    }

    if (move_type == CASTLING) {
        this->handleCastle();
    }

    // Update bitboards.
    this->kings &= ~(1ULL << start);
    this->kings |= 1ULL << end;
}

/**
 * Make the queen move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Pos::makeQueenMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType queen = this->turn ? W_QUEEN : B_QUEEN;
    this->findAndRemovePiece(queen, (Square) start);
    this->addPiece(queen, (Square) end);
    this->queens &= ~(1ULL << start);
    this->queens |= 1ULL << end;
}

/**
 * Make the rook move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Pos::makeRookMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType rook = this->turn ? W_ROOK : B_ROOK;
    this->findAndRemovePiece(rook, (Square) start);
    this->addPiece(rook, (Square) end);
    this->rooks &= ~(1ULL << start);
    this->rooks |= 1ULL << end;
}

/**
 * Make the bishop move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Pos::makeBishopMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType bishop = this->turn ? W_BISHOP : B_BISHOP;
    this->findAndRemovePiece(bishop, (Square) start);
    this->addPiece(bishop, (Square) end);
    this->bishops &= ~(1ULL << start);
    this->bishops |= 1ULL << end;
}

/**
 * Make the knight move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Pos::makeKnightMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType knight = this->turn ? W_KNIGHT : B_KNIGHT;
    this->findAndRemovePiece(knight, (Square) start);
    this->addPiece(knight, (Square) end);
    this->knights &= ~(1ULL << start);
    this->knights |= 1ULL << end;
}

/**
 * Prints a readable version of a move.
 * @param move: The move to be printed.
 */
void printMove(Move move, bool extraInfo) {
    if (extraInfo) {
        std::cout << squareName[move & 0b111111] << squareName[(move >> 6) & 0b111111] << " " <<
                moveName[(move >> 12) & 0b11] << " " << promoName[(move >> 14) & 0b11] << '\n';
    } else if ((move >> 12) & 0b11) {
        std::cout << squareName[move & 0b111111] << squareName[(move >> 6) &0b111111] <<
                promoName[(move >> 14) & 0b11] << '\n';
    } else {
        std::cout << squareName[move & 0b111111] << squareName[(move >> 6) & 0b111111] << '\n';
    }
}

/**
 * Make the pawn move.
 * @param move: The move to make.
 * @param game: Pointer to game struct.
 */
void Pos::makePawnMoves(Move move) {
    this->removePiece();
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    int move_type = move & (0b11 << 12);
    int promo = move & (0b11 << 14);
    bool pawn = false;
    int ep = end + (this->turn ? -8 : 8);
    PieceType piece = this->turn ? W_PAWN : B_PAWN;
    this->findAndRemovePiece(piece, (Square) start);

    if (move_type == PROMOTION) {
        if (this->turn) {
            piece = promo == pQUEEN ? W_QUEEN : promo == pROOK ? 
                W_ROOK : promo == pBISHOP ? W_BISHOP : W_KNIGHT;
        } else {
            piece = promo == pQUEEN ? B_QUEEN : promo == pROOK ? 
                B_ROOK : promo == pBISHOP ? B_BISHOP : B_KNIGHT;
        }

        if (promo == pQUEEN) {
            this->queens |= 1ULL << end;
            PieceType queen = this->turn ? W_QUEEN : B_QUEEN;
            this->addPiece(queen, (Square) end);
        } else if (promo == pROOK) {
            this->rooks |= 1ULL << end;
            PieceType ROOK = this->turn ? W_ROOK : B_ROOK;
            this->addPiece(ROOK, (Square) end);
        } else if (promo == pBISHOP) {
            this->bishops |= 1ULL << end;
            PieceType bishop = this->turn ? W_BISHOP : B_BISHOP;
            this->addPiece(bishop, (Square) end);
        } else {
            this->knights |= 1ULL << end;
            PieceType knight = this->turn ? W_KNIGHT : B_KNIGHT;
            this->addPiece(knight, (Square) end);
        }
        this->pieces[end] = piece;
        this->hash ^= Hashes::PIECES[piece][end];
    } else if (move_type == EN_PASSANT) {
        pawn = true;
        this->addPiece(piece, (Square) end);
        piece = this->turn ? B_PAWN : W_PAWN;
        this->findAndRemovePiece(piece, (Square) ep);
        this->pawns &= ~(1ULL << ep);
        this->sides[1 - this->turn] &= ~(1ULL << ep);
        this->pieces[ep] = NO_PIECE;
        this->hash ^= Hashes::PIECES[piece][ep];
    } else {
        pawn = true;
        this->addPiece(piece, (Square) end);
        int rank_diff = end / 8 - start / 8;
        if (std::abs(rank_diff) != 1) {
            this->en_passant = (Square) (start + 8 * (rank_diff - (rank_diff > 0 ? 1 : -1)));
            this->hash ^= Hashes::EN_PASSANT[this->en_passant % 8];
        }
    }

    this->pawns &= ~(1ULL << start);
    this->pawns |= (uint64_t)pawn << end;
}

/**
 * Save game state to history for undoing moves.
 * @param game: Pointer to game struct.
 * @param move: The move to made.
 */
void Pos::saveHistory(Move move) {
    this->history[this->ply].castling = this->castling;
    this->history[this->ply].en_passant = this->en_passant;
    this->history[this->ply].halfmove = this->halfmove;
    this->history[this->ply].move = move;
    this->history[this->ply].captured = this->pieces[(move >> 6) & 0b111111];
    this->history[this->ply].hash = this->hash;
    this->ply++;
}

/**
 * Undo normal moves.
 * @param game: Pointer to game struct.
 */
void Pos::undoNormal() {
    // Change game history
    this->ply--;
    History previous_pos = this->history[this->ply];
    
    // Retrieve last move information
    Move move = previous_pos.move;
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType moved = this->pieces[end];
    PieceType captured = previous_pos.captured;
    
    // Change turn
    this->turn = 1 - this->turn;
    bool turn = this->turn;

    // Change sides bitboards
    this->sides[turn] &= ~(1ULL << end);
    this->sides[turn] |= 1ULL << start;
    if (captured != NO_PIECE) this->sides[!turn] |= 1ULL << end;

    // Change pieces bitboards, piece list and indices and piece counts
    if (moved == B_PAWN || moved == W_PAWN) {
        this->pawns &= ~(1ULL << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->pawns |= 1ULL << start;
        this->addPiece(moved, (Square) start);
    } else if (moved == B_KNIGHT || moved == W_KNIGHT) {
        this->knights &= ~(1ULL << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->knights |= 1ULL << start;
        this->addPiece(moved, (Square) start);
    } else if (moved == B_BISHOP || moved == W_BISHOP) {
        this->bishops &= ~(1ULL << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->bishops |= 1ULL << start;
        this->addPiece(moved, (Square) start);
    } else if (moved == B_ROOK || moved == W_ROOK) {
        this->rooks &= ~(1ULL << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->rooks |= 1ULL << start;
        this->addPiece(moved, (Square) start);
    } else if (moved == B_QUEEN || moved == W_QUEEN) {
        this->queens &= ~(1ULL << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->queens |= 1ULL << start;
        this->addPiece(moved, (Square) start);
    } else {
        this->kings &= ~(1ULL << end);
        this->findAndRemovePiece(moved, (Square) end);
        this->kings |= 1ULL << start;
        this->addPiece(moved, (Square) start);
    }

    if (captured == NO_PIECE) {
        /* Do nothing */
    } else if (captured == B_PAWN || captured == W_PAWN) {
        this->pawns |= 1ULL << end;
        this->addPiece(captured, (Square) end);
        this->piece_cnt++;
    } else if (captured == B_KNIGHT || captured == W_KNIGHT) {
        this->knights |= 1ULL << end;
        this->addPiece(captured, (Square) end);
        this->piece_cnt++;
        this->knight_cnt++;
    } else if (captured == B_BISHOP || captured == W_BISHOP) {
        this->bishops |= 1ULL << end;
        addPiece(captured, (Square) end);
        this->piece_cnt++;
        if (isDark(end)) {
            turn ? this->bdsb_cnt++ : this->wdsb_cnt++;
        } else {
            turn ? this->blsb_cnt++ : this->wlsb_cnt++;
        }
    } else if (captured == B_ROOK || captured == W_ROOK) {
        this->rooks |= 1ULL << end;
        this->addPiece(captured, (Square) end);
        this->piece_cnt++;
    } else {
        this->queens |= 1ULL << end;
        this->addPiece(captured, (Square) end);
        this->piece_cnt++;
    }

    // Undo fullmove and History struct information
    if (!turn) this->fullmove--;
    this->castling = previous_pos.castling;
    this->en_passant = previous_pos.en_passant;
    this->halfmove = previous_pos.halfmove;
    this->hash = previous_pos.hash;

    // Update pieces array
    this->pieces[start] = moved;
    this->pieces[end] = captured;
}

/**
 * Undo promotion moves.
 * @param game: Pointer to game struct.
 */
void Pos::undoPromotion() {
    // Change game history
    this->ply--;
    History previous_pos = this->history[this->ply];

    // Change turn
    this->turn = 1 - this->turn;
    bool turn = this->turn;

    // Retrieve last move information
    Move move = previous_pos.move;
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType promoted = this->pieces[end];
    PieceType captured = previous_pos.captured;
    PieceType pawn = turn ? W_PAWN : B_PAWN;

    // Remove promoted piece
    this->findAndRemovePiece(promoted, (Square) end);
    this->sides[turn] &= ~(1ULL << end);
    this->pieces[end] = NO_PIECE;
    if (promoted == W_QUEEN || promoted == B_QUEEN) {
        this->queens &= ~(1ULL << end);
    } else if (promoted == W_ROOK || promoted == B_ROOK) {
        this->rooks &= ~(1ULL << end);
    } else if (promoted == W_BISHOP || promoted == B_BISHOP) {
        this->bishops &= ~(1ULL << end);
    } else { // Knight
        this->knights &= ~(1ULL << end);
    }

    // Replace captured piece (if any)
    if (captured != NO_PIECE) {
        this->addPiece(captured, (Square) end);
        this->sides[!turn] |= 1ULL << end;
        this->pieces[end] = captured;
        if (captured == W_QUEEN || captured == B_QUEEN) {
            this->queens |= 1ULL << end;
        } else if (captured == W_ROOK || captured == B_ROOK) {
            this->rooks |= 1ULL << end;
        } else if (captured == W_BISHOP || captured == B_BISHOP) {
            this->bishops |= 1ULL << end;
        } else { // Knight
            this->knights |= 1ULL << end;
        }
    }

    // Replace pawn
    this->addPiece(pawn, (Square) start);
    this->sides[turn] |= 1ULL << start;
    this->pawns |= 1ULL << start;
    this->pieces[start] = pawn;

    // Undo fullmove and History struct information
    if (!turn) this->fullmove--;
    this->castling = previous_pos.castling;
    this->en_passant = previous_pos.en_passant;
    this->halfmove = previous_pos.halfmove;
    this->hash = previous_pos.hash;
}

/**
 * Undo en-passant moves.
 * @param game: Pointer to game struct.
 */
void Pos::undoEnPassant() {
    // Change game history
    this->ply--;
    History previous_pos = this->history[this->ply];

    // Change turn
    this->turn = 1 - this->turn;
    bool turn = this->turn;

    // Retrieve last move information
    Move move = previous_pos.move;
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType moved = this->pieces[end];
    PieceType captured = turn ? B_PAWN : W_PAWN;
    int captured_sq = end + (turn ? -8 : 8);

    // Change sides bitboards
    this->sides[turn] &= ~(1ULL << end);
    this->sides[turn] |= 1ULL << start;
    this->sides[!turn] |= 1ULL << captured_sq;

    // Change pawn bitboards
    this->pawns &= ~(1ULL << end);
    this->pawns |= 1ULL << start;
    this->pawns |= 1ULL << captured_sq;

    // Undo fullmove and History struct information
    if (!turn) this->fullmove--;
    this->castling = previous_pos.castling;
    this->en_passant = previous_pos.en_passant;
    this->halfmove = previous_pos.halfmove;
    this->hash = previous_pos.hash;

    // Update piece list, indices and counts
    this->findAndRemovePiece(moved, (Square) end);
    this->addPiece(moved, (Square) start);
    this->addPiece(captured, (Square) captured_sq);
    this->piece_cnt++;

    // Update pieces array
    this->pieces[start] = moved;
    this->pieces[end] = NO_PIECE;
    this->pieces[captured_sq] = captured;
}

/**
 * Undo castling moves.
 * @param game: Pointer to game struct.
 */
void Pos::undoCastling() {
    // Change game history
    this->ply--;
    History previous_pos = this->history[this->ply];

    // Change turn
    this->turn = 1 - this->turn;
    bool turn = this->turn;

    // Retrieve last move information
    Move move = previous_pos.move;
    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    PieceType moved = this->pieces[end];

    // Rook changes
    int rook_start, rook_end;
    PieceType rook = turn ? W_ROOK : B_ROOK;
    if (end == G1) {
        rook_start = H1;
        rook_end = F1;
    } else if (end == C1) {
        rook_start = A1;
        rook_end = D1;
    } else if (end == G8) {
        rook_start = H8;
        rook_end = F8;
    } else {
        rook_start = A8;
        rook_end = D8;
    }

    // Change sides bitboards
    this->sides[turn] &= ~(1ULL << end);
    this->sides[turn] |= 1ULL << start;
    this->sides[turn] &= ~(1ULL << rook_end);
    this->sides[turn] |= 1ULL << rook_start;

    // Change king bitboards
    this->kings &= ~(1ULL << end);
    this->kings |= 1ULL << start;

    // Change rook bitboards
    this->rooks &= ~(1ULL << rook_end);
    this->rooks |= 1ULL << rook_start;

    // Undo fullmove and History struct information (some is not necessary i think? Done out of principle)
    if (!turn) this->fullmove--;
    this->castling = previous_pos.castling;
    this->en_passant = previous_pos.en_passant;
    this->halfmove = previous_pos.halfmove;
    this->hash = previous_pos.hash;

    // Update piece list, indices and counts
    this->findAndRemovePiece(moved, (Square) end);
    this->addPiece(moved, (Square) start);
    this->findAndRemovePiece(rook, (Square) rook_end);
    this->addPiece(rook, (Square) rook_start);

    // Update pieces array
    this->pieces[start] = moved;
    this->pieces[end] = NO_PIECE;
    this->pieces[rook_start] = rook;
    this->pieces[rook_end] = NO_PIECE;
}

/**
 * Decrements the position hash counter for undo moves.
 * @param hash: The hash to decrement.
 */
void Pos::decrementHash(const Bitboard hash) {
    auto it = this->hashes.find(hash);
    if (it != this->hashes.end()) {
        this->hashes[hash]--;
    }
}

/**
 * Undo moves.
 * @param game: Pointer to game struct.
 */
void Pos::undoMove() {
    if (this->ply == 0) {
        std::cout << "Nothing to undo...\n";
        return;
    }

    this->decrementHash(this->hash);
    int type = this->history[this->ply - 1].move & (0b11 << 12);
    if (type == NORMAL) {
        this->undoNormal();
    } else if (type == PROMOTION) {
        this->undoPromotion();
    } else if (type == EN_PASSANT) {
        this->undoEnPassant();
    } else {
        this->undoCastling();
    }
}

/**
 * Make the moves.
 * @param move: The moves to make.
 * @param game: Pointer to game struct.
 */
void Pos::makeMove(Move move) {
    if (move == 0) {
        this->undoMove();
        return;
    }

    // Save current position and move to make to history.
    this->saveHistory(move);

    int start = move & 0b111111;
    int end = (move >> 6) & 0b111111;
    int move_type = move & (0b11 << 12);
    
    this->last_move = move;
    this->last_move_type = (MoveType) move_type;
    this->piece_captured = this->pieces[end];
    this->piece_moved = this->pieces[start];

    // Remove en-passant and its hash
    if (this->en_passant != NONE) {
        this->hash ^= Hashes::EN_PASSANT[this->en_passant % 8];
        this->en_passant = NONE;
    }

    this->sides[this->turn] &= ~(1ULL << start);
    this->sides[this->turn] |= 1ULL << end;
    this->sides[1 - this->turn] &= ~(1ULL << end);
    this->pieces[start] = NO_PIECE;
    this->hash ^= Hashes::PIECES[this->piece_moved][start];
    this->pieces[end] = this->piece_moved;
    if (this->piece_captured != NO_PIECE) {
        this->hash ^= Hashes::PIECES[this->piece_captured][end];
    }
    this->hash ^= Hashes::PIECES[this->piece_moved][end];

    // Change castling privileges.
    if (start == E1 && 0b11 & this->castling) {
        this->hash ^= Hashes::CASTLING[this->castling];
        this->castling &= 0b1100;
        this->hash ^= Hashes::CASTLING[this->castling];
    }

    if (start == E8 && (0b11 << 2) & this->castling) {
        this->hash ^= Hashes::CASTLING[this->castling];
        this->castling &= 0b0011;
        this->hash ^= Hashes::CASTLING[this->castling];
    }

    if ((start == H1 || end == H1) && this->castling & (1 << WKSC)) {
        this->hash ^= Hashes::CASTLING[this->castling];
        this->castling &= ~(1 << WKSC);
        this->hash ^= Hashes::CASTLING[this->castling];
    }

    if ((start == A1 || end == A1) && this->castling & (1 << WQSC)) {
        this->hash ^= Hashes::CASTLING[this->castling];
        this->castling &= ~(1 << WQSC);
        this->hash ^= Hashes::CASTLING[this->castling];
    }

    if ((start == H8 || end == H8) && this->castling & (1 << BKSC)) {
        this->hash ^= Hashes::CASTLING[this->castling];
        this->castling &= ~(1 << BKSC);
        this->hash ^= Hashes::CASTLING[this->castling];
    }

    if ((start == A8 || end == A8) && this->castling & (1 << BQSC)) {
        this->hash ^= Hashes::CASTLING[this->castling];
        this->castling &= ~(1 << BQSC);
        this->hash ^= Hashes::CASTLING[this->castling];
    }

    PieceType moved = this->piece_moved;
    if (moved == W_KING || moved == B_KING) {
        this->makeKingMoves(move);
    } else if (moved == W_QUEEN || moved == B_QUEEN) {
        this->makeQueenMoves(move);
    } else if (moved == W_ROOK || moved == B_ROOK) {
        this->makeRookMoves(move);
    } else if (moved == W_BISHOP || moved == B_BISHOP) {
        this->makeBishopMoves(move);
    } else if (moved == W_KNIGHT || moved == B_KNIGHT) {
        this->makeKnightMoves(move);
    } else {
        this->makePawnMoves(move);
    }

    this->turn = 1 - this->turn;
    if (this->turn == BLACK) {
        this->fullmove++;
        this->hash ^= Hashes::TURN;
    }
    if (moved == W_PAWN || moved == B_PAWN || this->piece_captured != NO_PIECE) {
        this->halfmove = 0;
    } else {
        this->halfmove++;
    }

    // Increment position hash counter
    this->incrementHash(move);
}

void Pos::incrementHash(Move move) {
    auto record = this->hashes.find(this->hash);
    if (record != this->hashes.end()) {
        this->hashes[this->hash]++;
    } else {
        this->hashes.insert(std::pair<Bitboard, int>(this->hash, 1));
    }
}

/**
 * Count and print the moves of the current position.
 * @param game: Pointer to game struct.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to int of number of move vectors in pos_moves.
 * @return: The number of moves in the position.
 */
int countMoves(Pos* game, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index) {
    int count = 0;
    for (int i = 0; i < *moves_index; i++) {
        std::vector<Move>* pointer = pos_moves[i];
        for (Move move : *pointer) {
            std::cout << squareName[move & 0b111111] << " " << squareName[(move >> 6) & 0b111111] << " " <<
                    moveName[(move >> 12) & 0b11] << " " << promoName[(move >> 14) & 0b11] << '\n';
            count++;
        }
    }
    return count;
}

/**
 * Checks if a given move is valid.
 * @param move: The move to check.
 * @param game: Pointer to game move struct.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Int pointer to number of vectors in pos_moves.
 * @return: True if move is valid, else false.
 */
bool Pos::validMove(Move move, std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index) {
    for (int i = 0; i < *moves_index; i++) {
        for (Move move_candidate : *pos_moves[i]) {
            if (move == move_candidate) return true;
        }
    }
    return false;
}

/**
 * Get and return the FEN string of the current position.
 * @param game: Pointer to game struct.
 */
std::string Pos::getFEN() {
    std::string fen = "";
    int no_piece_count = 0;
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file <= 7; file++) {
            int sq = rank * 8 + file;
            
            if (sq % 8 == 0 && sq != A8) {
                if (no_piece_count != 0) {
                    fen += no_piece_count + '0';
                    no_piece_count = 0;
                }
                fen += "/";
            }
            
            PieceType piece = this->pieces[sq];
            if (piece == NO_PIECE) {
                no_piece_count++;
            } else {
                if (no_piece_count != 0) {
                    fen += no_piece_count + '0';
                    no_piece_count = 0;
                }
                if (piece == B_KING) fen += 'k';
                else if (piece == B_QUEEN) fen += 'q';
                else if (piece == B_ROOK) fen += 'r';
                else if (piece == B_BISHOP) fen += 'b';
                else if (piece == B_KNIGHT) fen += 'n';
                else if (piece == B_PAWN) fen += 'p';
                else if (piece == W_KING) fen += 'K';
                else if (piece == W_QUEEN) fen += 'Q';
                else if (piece == W_ROOK) fen += 'R';
                else if (piece == W_BISHOP) fen += 'B';
                else if (piece == W_KNIGHT) fen += 'N';
                else fen += 'P';
            }
        }
    }
    if (no_piece_count != 0) {
        fen += no_piece_count + '0';
        no_piece_count = 0;
    }

    if (this->turn) {
        fen += " w ";
    } else {
        fen += " b ";
    }

    if (this->castling == 0) {
        fen += "-";
    } else {
        if (this->castling & (1 << WKSC)) fen += "K";
        if (this->castling & (1 << WQSC)) fen += "Q";
        if (this->castling & (1 << BKSC)) fen += "k";
        if (this->castling & (1 << BQSC)) fen += "q";
    }
    fen += " ";

    if (this->en_passant == NONE) {
        fen += "- ";
    } else {
        fen += squareName[this->en_passant] + " ";
    }

    fen += this->halfmove + '0';
    fen += " ";
    fen += this->fullmove + '0';

    return fen;
}

/**
 * Gets the start and end squares of a move. Also sets the promotion moves.
 * @param move_string: The move string.
 * @param move: Unsigned 16-bit int representing the move.
 * @param start: Pointer to unsigned int for start square.
 * @param end: Pointer to unsigned int for end square.
 */
void Pos::getSquares(std::string move_string, Move* move, uint* start, uint* end) {
    int start_file, start_rank, end_file, end_rank;
    start_file = move_string[0] - 'a';
    start_rank = move_string[1] - '1';
    end_file = move_string[2] - 'a';
    end_rank = move_string[3] - '1';
    if (move_string.length() == 5) {
        *move |= PROMOTION;
        if (move_string[4] == 'q') {
            *move |= pQUEEN;
        } else if (move_string[4] == 'r') {
            *move |= pROOK;
        } else if (move_string[4] == 'b') {
            *move |= pBISHOP;
        }
    }

    *start = 8 * start_rank + start_file;
    *end = 8 * end_rank + end_file;
}

/**
 * Checks and sets castling and en passant moves, if any.
 * @param game: Pointer to the game struct.
 * @param start: The start square.
 * @param end: The end square.
 * @param move: Pointer to the integer representing the move.
 */
void Pos::checkCastlingEnPassantMoves(uint start, uint end, Move* move) {
    // Check for castling move.
    if (this->turn && this->piece_list[WHITE][0] == E1) {
        if (start == E1 && end == G1 && (this->castling & (1 << WKSC))) {
            *move |= CASTLING;
        } else if (start == E1 && end == C1 && (this->castling & (1 << WQSC))) {
            *move |= CASTLING;
        }
    } else if (!this->turn && this->piece_list[BLACK][0] == E8) {
        if (start == E8 && end == G8 && (this->castling & (1 << BKSC))) {
            *move |= CASTLING;
        } else if (start == E8 && end == C8 && (this->castling & (1 << BQSC))) {
            *move |= CASTLING;
        }
    }

    // Check for en-passant move.
    if ((this->turn && start / 8 == 4) || (!this->turn && start / 8 == 3)) {
        PieceType piece = this->pieces[start];
        if ((piece == W_PAWN || piece == B_PAWN) && end == this->en_passant) {
            *move |= EN_PASSANT;
        }
    }
}

/**
 * Process human move choice or generate random move choice for computer.
 * @param game: Pointer to game move struct.
 * @param white: The type of player for white.
 * @param black: The type of player for black.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Int pointer to number of vectors in pos_moves.
 * @return: The chosen move.
 */
Move Pos::chooseMove(std::vector<Move>* pos_moves[MAX_MOVE_SETS], int* moves_index) {
    Move move = NORMAL | pKNIGHT;

    // Recieve and print move.
    if ((this->turn && this->white == HUMAN) || (!this->turn && this->black == HUMAN)) {
        std::cout << "Enter move: ";

        while (true) {
            move = NORMAL | pKNIGHT;
            std::string move_string;
            std::cin >> move_string;

            if (move_string == "kill") exit(-1);
            if (move_string == "fen") std::cout << this->getFEN() << '\n';
            if (move_string == "displayall") this->displayAll();
            if (move_string == "undo") return 0;

            uint start, end;
            this->getSquares(move_string, &move, &start, &end);
            this->checkCastlingEnPassantMoves(start, end, &move);

            move |= start;
            move |= (end << 6);
            if (!this->validMove(move, pos_moves, moves_index)) {
                std::cout << "Invalid move, enter again: ";
            } else break;
        }
         
    } else {
        // std::vector<Move>* pos_moves[MAX_MOVE_SETS];
        // int moves_index;
        // this->getMoves(moves_index, pos_moves);
        // double value = this->turn ? -1000000 : 1000000;
        // std::cout << "Thinking..." << std::flush;
        // for (int i = 0; i < moves_index; i++) {
        //     std::vector<Move>* move_set = pos_moves[i];
        //     for (Move move_candidate : *move_set) {
        //         this->makeMove(move_candidate);
        //         double pos_value = this->alphaBeta(4, -100000, 100000, this->turn);
        //         if (pos_value > value && !this->turn) {
        //             move = move_candidate;
        //             value = pos_value;
        //         } else if (pos_value < value && this->turn) {
        //             move = move_candidate;
        //             value = pos_value;
        //         }
        //         this->undoMove();
        //     }
        // }
        // std::cout << "\rComputer move: ";
        // printMove(move, true);
    }
    
    return move;
}

/**
 * Handles a single game.
 */
void Pos::run() {
    ExitCode code = NORMAL_PLY;
    std::vector<Move>* pos_moves[MAX_MOVE_SETS];
    int moves_index;
    this->getMoves(moves_index, pos_moves);

    while (!(code = this->isEOG(moves_index))) {
        this->display();
        this->makeMove(this->chooseMove(pos_moves, &moves_index));
        this->getMoves(moves_index, pos_moves);
    }

    this->display();
    this->showEOG(code);
}

/**
 * Prints the promotion moves for the leaf nodes.
 * @param move: The moves to print for.
 */
void printPromo(Move move) {
    if ((move & (0b11 << 12)) == PROMOTION) {
        std::cout << squareName[move & 0b111111] << squareName[(move >> 6) & 0b111111];
        if ((move & (0b11 << 14)) == pKNIGHT) {
            std::cout << "n: 1\n";
        } else if ((move & (0b11 << 14)) == pBISHOP) {
            std::cout << "b: 1\n";
        } else if ((move & (0b11 << 14)) == pROOK) {
            std::cout << "r: 1\n";
        } else {
            std::cout << "q: 1\n";
        }
    } else if ((move & (0b11 << 12)) == EN_PASSANT) {
        std::cout << squareName[move & 0b111111] << squareName[(move >> 6) & 0b111111] << ": 1\n";
    } else {
        std::cout << squareName[move & 0b111111] << squareName[(move >> 6) & 0b111111] << ": 1\n";
    }
}

/**
 * Prints the perft move divide.
 * @param print: Boolean to indicate whether or not to print.
 * @param move: Move to print for.
 * @param current_nodes: Number of leaf nodes from playing move.
 */
void printPerft(bool print, Move move, uint64_t* current_nodes) {
    if (print) {
        if ((move & (0b11 << 12)) == PROMOTION) {
            std::cout << squareName[move & 0b111111] << squareName[(move >> 6) & 0b111111];
            if ((move & (0b11 << 14)) == pKNIGHT) {
                std::cout << "n: " << *current_nodes << '\n';
            } else if ((move & (0b11 << 14)) == pBISHOP) {
                std::cout << "b: " << *current_nodes << '\n';
            } else if ((move & (0b11 << 14)) == pROOK) {
                std::cout << "r: " << *current_nodes << '\n';
            } else {
                std::cout << "q: " << *current_nodes << '\n';
            }
        } else if ((move & (0b11 << 12)) == EN_PASSANT) {
            std::cout << squareName[move & 0b111111] << squareName[(move >> 6) & 0b111111] << ": " << *current_nodes <<
                    '\n';
        } else {
            std::cout << squareName[move & 0b111111] << squareName[(move >> 6) & 0b111111] << ": " << *current_nodes <<
                    '\n';
        }
    }
}

/**
 * Runs perft testing.
 * @param depth: The depth to search to (not including current depth).
 * @param game: Pointer to game struct.
 * @param print: Boolean to indicate whether or not to print.
 */
uint64_t Pos::perft(int depth, bool print) {
    uint64_t nodes = 0;
    std::vector<Move>* pos_moves[MAX_MOVE_SETS];
    int moves_index;
    this->getMoves(moves_index, pos_moves);

    for (int i = 0; i < moves_index; i++) {
        std::vector<Move>* move_set = pos_moves[i];
        for (Move move : *move_set) {
            uint64_t current_nodes = 0;
            if (depth == 1) {
                nodes += move_set->size();
                if (print) {
                    for (Move move2 : *move_set) {
                        printPromo(move2);
                    }
                }
                break;
            } else {
                this->makeMove(move);
                current_nodes = perft(depth - 1);
            }

            printPerft(print, move, &current_nodes);

            nodes += current_nodes;
            this->undoMove();
        }
    }
    
    return nodes;
}

/**
 * Prepares and makes call to run perft.
 */
void runPerft(int depth, Pos game) {
    uint64_t num = game.perft(depth, true);
    std::cout << "\nNodes searched: " << num << "\n\n";
}

/**
 * Makes call to run a game instance. Takes pos by value to simplify runNormal loop.
 */
void handleGame(Pos pos) {
    pos.run();
}

/**
 * Pos class constructor.
 */
Pos::Pos(std::string fen) {
    this->parseFen(fen);
    this->initialiseHash();
}

/**
 * Concatenates vector of strings with space.
 */
std::string concatFEN(std::vector<std::string> strings) {
    std::string result = "";
    for (int i = 2; i < (int) strings.size(); i++) {
        result += strings[i] + " ";
    }
    return result;
}

/**
 * Sets the position of the pos object.
 */
void setFen(std::vector<std::string> commands, Pos& pos) {
    if (commands[2] == "kiwipete") {
        pos.parseFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    } else if (commands[2] == "new") {
        pos.parseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    } else {
        pos.parseFen(concatFEN(commands));
    }
}

/**
 * Loop for user input tasks.
 * @param input: The initial user input.
 */
void runNormal(std::string input) {
    Pos pos;
    while (input != "exit" && input != "quit" && input != "q") {
        std::vector<std::string> commands = split(input, " ");
        if (commands[0] == "play") handleGame(pos);
        if (commands[0] == "perft") runPerft(std::stoi(commands[1]), pos);
        if (commands[0] == "set" && commands[1] == "fen") setFen(commands, pos);
        if (commands[0] == "display" && commands.size() == 1) pos.display();
        if (commands[0] == "display" && commands[1] == "all") pos.displayAll();
        if (commands[0] == "exit" || commands[0] == "quit" || commands[0] == "q") break;
        std::getline(std::cin, input);
    }
}

void Play::init(std::string input) {
    runNormal(input);
}
