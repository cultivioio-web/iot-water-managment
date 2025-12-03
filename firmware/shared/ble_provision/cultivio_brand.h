/*
 * Cultivio Brand Header
 * Company branding for Water Management System
 * 
 * Copyright (c) 2024 Cultivio
 * All rights reserved.
 */

#ifndef CULTIVIO_BRAND_H
#define CULTIVIO_BRAND_H

/* ============================================================================
 * COMPANY INFORMATION
 * ============================================================================ */

#define CULTIVIO_COMPANY_NAME       "Cultivio"
#define CULTIVIO_PRODUCT_NAME       "AquaSense"
#define CULTIVIO_PRODUCT_VERSION    "1.0.0"
#define CULTIVIO_COPYRIGHT          "Copyright (c) 2024 Cultivio"
#define CULTIVIO_WEBSITE            "www.cultivio.com"

/* ============================================================================
 * ASCII ART LOGO
 * ============================================================================ */

#define CULTIVIO_LOGO_ASCII \
"                                                                \n" \
"   ██████╗██╗   ██╗██╗  ████████╗██╗██╗   ██╗██╗ ██████╗       \n" \
"  ██╔════╝██║   ██║██║  ╚══██╔══╝██║██║   ██║██║██╔═══██╗      \n" \
"  ██║     ██║   ██║██║     ██║   ██║██║   ██║██║██║   ██║      \n" \
"  ██║     ██║   ██║██║     ██║   ██║╚██╗ ██╔╝██║██║   ██║      \n" \
"  ╚██████╗╚██████╔╝███████╗██║   ██║ ╚████╔╝ ██║╚██████╔╝      \n" \
"   ╚═════╝ ╚═════╝ ╚══════╝╚═╝   ╚═╝  ╚═══╝  ╚═╝ ╚═════╝       \n" \
"                                                                \n"

#define CULTIVIO_LOGO_SIMPLE \
"   ╔═══════════════════════════════════════════════════════╗   \n" \
"   ║                                                       ║   \n" \
"   ║             ██████╗ Cultivio AquaSense               ║   \n" \
"   ║            ██╔════╝ Smart Water Management           ║   \n" \
"   ║            ██║      ═══════════════════              ║   \n" \
"   ║            ╚██████╗ IoT Water Level System           ║   \n" \
"   ║             ╚═════╝ v1.0.0                           ║   \n" \
"   ║                                                       ║   \n" \
"   ╚═══════════════════════════════════════════════════════╝   \n"

#define CULTIVIO_BOOT_BANNER \
"\n" \
"╔════════════════════════════════════════════════════════════════╗\n" \
"║                                                                ║\n" \
"║      ██████╗██╗   ██╗██╗  ████████╗██╗██╗   ██╗██╗ ██████╗     ║\n" \
"║     ██╔════╝██║   ██║██║  ╚══██╔══╝██║██║   ██║██║██╔═══██╗    ║\n" \
"║     ██║     ██║   ██║██║     ██║   ██║██║   ██║██║██║   ██║    ║\n" \
"║     ██║     ██║   ██║██║     ██║   ██║╚██╗ ██╔╝██║██║   ██║    ║\n" \
"║     ╚██████╗╚██████╔╝███████╗██║   ██║ ╚████╔╝ ██║╚██████╔╝    ║\n" \
"║      ╚═════╝ ╚═════╝ ╚══════╝╚═╝   ╚═╝  ╚═══╝  ╚═╝ ╚═════╝     ║\n" \
"║                                                                ║\n" \
"║                    ╔═══════════════════╗                       ║\n" \
"║                    ║    AquaSense      ║                       ║\n" \
"║                    ║  Water Management ║                       ║\n" \
"║                    ╚═══════════════════╝                       ║\n" \
"║                                                                ║\n" \
"║                      Version 1.0.0                             ║\n" \
"║              Copyright (c) 2024 Cultivio                       ║\n" \
"║                                                                ║\n" \
"╚════════════════════════════════════════════════════════════════╝\n\n"

/* Compact banner for serial output */
#define CULTIVIO_COMPACT_BANNER \
"\n" \
"  ╔════════════════════════════════════════════╗\n" \
"  ║     CULTIVIO AquaSense v1.0.0              ║\n" \
"  ║     Smart Water Management System          ║\n" \
"  ║     (c) 2024 Cultivio                      ║\n" \
"  ╚════════════════════════════════════════════╝\n\n"

/* ============================================================================
 * HELPER MACROS
 * ============================================================================ */

#define PRINT_CULTIVIO_BANNER()     printf(CULTIVIO_BOOT_BANNER)
#define PRINT_CULTIVIO_COMPACT()    printf(CULTIVIO_COMPACT_BANNER)

/* Product info string */
#define CULTIVIO_PRODUCT_INFO \
    CULTIVIO_COMPANY_NAME " " CULTIVIO_PRODUCT_NAME " v" CULTIVIO_PRODUCT_VERSION

#endif /* CULTIVIO_BRAND_H */

