//
// pch.h
// Header for standard system include files.
//

#pragma once

#include "Common/Common.h"
#include "SDK/json/json.hpp"

NLOHMANN_JSON_SERIALIZE_ENUM( DXGI_FORMAT, {
   {DXGI_FORMAT_UNKNOWN, nullptr},
   {TOKEN_PAIR(DXGI_FORMAT_UNKNOWN	                              )},
   {TOKEN_PAIR(DXGI_FORMAT_R32G32B32A32_TYPELESS                  )},
   {TOKEN_PAIR(DXGI_FORMAT_R32G32B32A32_FLOAT                     )},
   {TOKEN_PAIR(DXGI_FORMAT_R32G32B32A32_UINT                      )},
   {TOKEN_PAIR(DXGI_FORMAT_R32G32B32A32_SINT                      )},
   {TOKEN_PAIR(DXGI_FORMAT_R32G32B32_TYPELESS                     )},
   {TOKEN_PAIR(DXGI_FORMAT_R32G32B32_FLOAT                        )},
   {TOKEN_PAIR(DXGI_FORMAT_R32G32B32_UINT                         )},
   {TOKEN_PAIR(DXGI_FORMAT_R32G32B32_SINT                         )},
   {TOKEN_PAIR(DXGI_FORMAT_R16G16B16A16_TYPELESS                  )},
   {TOKEN_PAIR(DXGI_FORMAT_R16G16B16A16_FLOAT                     )},
   {TOKEN_PAIR(DXGI_FORMAT_R16G16B16A16_UNORM                     )},
   {TOKEN_PAIR(DXGI_FORMAT_R16G16B16A16_UINT                      )},
   {TOKEN_PAIR(DXGI_FORMAT_R16G16B16A16_SNORM                     )},
   {TOKEN_PAIR(DXGI_FORMAT_R16G16B16A16_SINT                      )},
   {TOKEN_PAIR(DXGI_FORMAT_R32G32_TYPELESS                        )},
   {TOKEN_PAIR(DXGI_FORMAT_R32G32_FLOAT                           )},
   {TOKEN_PAIR(DXGI_FORMAT_R32G32_UINT                            )},
   {TOKEN_PAIR(DXGI_FORMAT_R32G32_SINT                            )},
   {TOKEN_PAIR(DXGI_FORMAT_R32G8X24_TYPELESS                      )},
   {TOKEN_PAIR(DXGI_FORMAT_D32_FLOAT_S8X24_UINT                   )},
   {TOKEN_PAIR(DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS               )},
   {TOKEN_PAIR(DXGI_FORMAT_X32_TYPELESS_G8X24_UINT                )},
   {TOKEN_PAIR(DXGI_FORMAT_R10G10B10A2_TYPELESS                   )},
   {TOKEN_PAIR(DXGI_FORMAT_R10G10B10A2_UNORM                      )},
   {TOKEN_PAIR(DXGI_FORMAT_R10G10B10A2_UINT                       )},
   {TOKEN_PAIR(DXGI_FORMAT_R11G11B10_FLOAT                        )},
   {TOKEN_PAIR(DXGI_FORMAT_R8G8B8A8_TYPELESS                      )},
   {TOKEN_PAIR(DXGI_FORMAT_R8G8B8A8_UNORM                         )},
   {TOKEN_PAIR(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB                    )},
   {TOKEN_PAIR(DXGI_FORMAT_R8G8B8A8_UINT                          )},
   {TOKEN_PAIR(DXGI_FORMAT_R8G8B8A8_SNORM                         )},
   {TOKEN_PAIR(DXGI_FORMAT_R8G8B8A8_SINT                          )},
   {TOKEN_PAIR(DXGI_FORMAT_R16G16_TYPELESS                        )},
   {TOKEN_PAIR(DXGI_FORMAT_R16G16_FLOAT                           )},
   {TOKEN_PAIR(DXGI_FORMAT_R16G16_UNORM                           )},
   {TOKEN_PAIR(DXGI_FORMAT_R16G16_UINT                            )},
   {TOKEN_PAIR(DXGI_FORMAT_R16G16_SNORM                           )},
   {TOKEN_PAIR(DXGI_FORMAT_R16G16_SINT                            )},
   {TOKEN_PAIR(DXGI_FORMAT_R32_TYPELESS                           )},
   {TOKEN_PAIR(DXGI_FORMAT_D32_FLOAT                              )},
   {TOKEN_PAIR(DXGI_FORMAT_R32_FLOAT                              )},
   {TOKEN_PAIR(DXGI_FORMAT_R32_UINT                               )},
   {TOKEN_PAIR(DXGI_FORMAT_R32_SINT                               )},
   {TOKEN_PAIR(DXGI_FORMAT_R24G8_TYPELESS                         )},
   {TOKEN_PAIR(DXGI_FORMAT_D24_UNORM_S8_UINT                      )},
   {TOKEN_PAIR(DXGI_FORMAT_R24_UNORM_X8_TYPELESS                  )},
   {TOKEN_PAIR(DXGI_FORMAT_X24_TYPELESS_G8_UINT                   )},
   {TOKEN_PAIR(DXGI_FORMAT_R8G8_TYPELESS                          )},
   {TOKEN_PAIR(DXGI_FORMAT_R8G8_UNORM                             )},
   {TOKEN_PAIR(DXGI_FORMAT_R8G8_UINT                              )},
   {TOKEN_PAIR(DXGI_FORMAT_R8G8_SNORM                             )},
   {TOKEN_PAIR(DXGI_FORMAT_R8G8_SINT                              )},
   {TOKEN_PAIR(DXGI_FORMAT_R16_TYPELESS                           )},
   {TOKEN_PAIR(DXGI_FORMAT_R16_FLOAT                              )},
   {TOKEN_PAIR(DXGI_FORMAT_D16_UNORM                              )},
   {TOKEN_PAIR(DXGI_FORMAT_R16_UNORM                              )},
   {TOKEN_PAIR(DXGI_FORMAT_R16_UINT                               )},
   {TOKEN_PAIR(DXGI_FORMAT_R16_SNORM                              )},
   {TOKEN_PAIR(DXGI_FORMAT_R16_SINT                               )},
   {TOKEN_PAIR(DXGI_FORMAT_R8_TYPELESS                            )},
   {TOKEN_PAIR(DXGI_FORMAT_R8_UNORM                               )},
   {TOKEN_PAIR(DXGI_FORMAT_R8_UINT                                )},
   {TOKEN_PAIR(DXGI_FORMAT_R8_SNORM                               )},
   {TOKEN_PAIR(DXGI_FORMAT_R8_SINT                                )},
   {TOKEN_PAIR(DXGI_FORMAT_A8_UNORM                               )},
   {TOKEN_PAIR(DXGI_FORMAT_R1_UNORM                               )},
   {TOKEN_PAIR(DXGI_FORMAT_R9G9B9E5_SHAREDEXP                     )},
   {TOKEN_PAIR(DXGI_FORMAT_R8G8_B8G8_UNORM                        )},
   {TOKEN_PAIR(DXGI_FORMAT_G8R8_G8B8_UNORM                        )},
   {TOKEN_PAIR(DXGI_FORMAT_BC1_TYPELESS                           )},
   {TOKEN_PAIR(DXGI_FORMAT_BC1_UNORM                              )},
   {TOKEN_PAIR(DXGI_FORMAT_BC1_UNORM_SRGB                         )},
   {TOKEN_PAIR(DXGI_FORMAT_BC2_TYPELESS                           )},
   {TOKEN_PAIR(DXGI_FORMAT_BC2_UNORM                              )},
   {TOKEN_PAIR(DXGI_FORMAT_BC2_UNORM_SRGB                         )},
   {TOKEN_PAIR(DXGI_FORMAT_BC3_TYPELESS                           )},
   {TOKEN_PAIR(DXGI_FORMAT_BC3_UNORM                              )},
   {TOKEN_PAIR(DXGI_FORMAT_BC3_UNORM_SRGB                         )},
   {TOKEN_PAIR(DXGI_FORMAT_BC4_TYPELESS                           )},
   {TOKEN_PAIR(DXGI_FORMAT_BC4_UNORM                              )},
   {TOKEN_PAIR(DXGI_FORMAT_BC4_SNORM                              )},
   {TOKEN_PAIR(DXGI_FORMAT_BC5_TYPELESS                           )},
   {TOKEN_PAIR(DXGI_FORMAT_BC5_UNORM                              )},
   {TOKEN_PAIR(DXGI_FORMAT_BC5_SNORM                              )},
   {TOKEN_PAIR(DXGI_FORMAT_B5G6R5_UNORM                           )},
   {TOKEN_PAIR(DXGI_FORMAT_B5G5R5A1_UNORM                         )},
   {TOKEN_PAIR(DXGI_FORMAT_B8G8R8A8_UNORM                         )},
   {TOKEN_PAIR(DXGI_FORMAT_B8G8R8X8_UNORM                         )},
   {TOKEN_PAIR(DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM             )},
   {TOKEN_PAIR(DXGI_FORMAT_B8G8R8A8_TYPELESS                      )},
   {TOKEN_PAIR(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB                    )},
   {TOKEN_PAIR(DXGI_FORMAT_B8G8R8X8_TYPELESS                      )},
   {TOKEN_PAIR(DXGI_FORMAT_B8G8R8X8_UNORM_SRGB                    )},
   {TOKEN_PAIR(DXGI_FORMAT_BC6H_TYPELESS                          )},
   {TOKEN_PAIR(DXGI_FORMAT_BC6H_UF16                              )},
   {TOKEN_PAIR(DXGI_FORMAT_BC6H_SF16                              )},
   {TOKEN_PAIR(DXGI_FORMAT_BC7_TYPELESS                           )},
   {TOKEN_PAIR(DXGI_FORMAT_BC7_UNORM                              )},
   {TOKEN_PAIR(DXGI_FORMAT_BC7_UNORM_SRGB                         )},
   {TOKEN_PAIR(DXGI_FORMAT_AYUV                                   )},
   {TOKEN_PAIR(DXGI_FORMAT_Y410                                   )},
   {TOKEN_PAIR(DXGI_FORMAT_Y416                                   )},
   {TOKEN_PAIR(DXGI_FORMAT_NV12                                   )},
   {TOKEN_PAIR(DXGI_FORMAT_P010                                   )},
   {TOKEN_PAIR(DXGI_FORMAT_P016                                   )},
   {TOKEN_PAIR(DXGI_FORMAT_420_OPAQUE                             )},
   {TOKEN_PAIR(DXGI_FORMAT_YUY2                                   )},
   {TOKEN_PAIR(DXGI_FORMAT_Y210                                   )},
   {TOKEN_PAIR(DXGI_FORMAT_Y216                                   )},
   {TOKEN_PAIR(DXGI_FORMAT_NV11                                   )},
   {TOKEN_PAIR(DXGI_FORMAT_AI44                                   )},
   {TOKEN_PAIR(DXGI_FORMAT_IA44                                   )},
   {TOKEN_PAIR(DXGI_FORMAT_P8                                     )},
   {TOKEN_PAIR(DXGI_FORMAT_A8P8                                   )},
   {TOKEN_PAIR(DXGI_FORMAT_B4G4R4A4_UNORM                         )},
   {TOKEN_PAIR(DXGI_FORMAT_P208                                   )},
   {TOKEN_PAIR(DXGI_FORMAT_V208                                   )},
   {TOKEN_PAIR(DXGI_FORMAT_V408                                   )},
   {TOKEN_PAIR(DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE        )},
   {TOKEN_PAIR(DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE)}
});

