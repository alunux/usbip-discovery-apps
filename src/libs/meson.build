sources = [
    'names.c',
    'usbip_device_driver.c',
    'usbip_host_common.c',
    'usbip_host_driver.c',
    'usbip_common.c',
    'vhci_driver.c',
    'sysfs_utils.c',
    'detect_iface.c',
]

extra_cflags = [
    '-Wno-variadic-macros',
]

deps = [
    require_libudev
]

ver = '0.1.0'

libs_headers = [
    'detect_iface.h',
    'usbip_common.h',
    'usbip_host_common.h',
    'usbip_device_driver.h',
    'usbip_host_driver.h',
    'vhci_driver.h',
]

install_headers(libs_headers)

usbip = shared_library(
    'usbip_app_libs', sources,
    c_args: extra_cflags,
    version : ver,
    dependencies : deps,
    install : true,
)
