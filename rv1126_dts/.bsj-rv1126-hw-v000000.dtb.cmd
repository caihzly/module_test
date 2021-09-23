cmd_arch/arm/boot/dts/bsj-rv1126-hw-v000000.dtb := mkdir -p arch/arm/boot/dts/ ; ./scripts/gcc-wrapper.py ./../prebuilts/gcc/linux-x86/arm/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc -E -Wp,-MD,arch/arm/boot/dts/.bsj-rv1126-hw-v000000.dtb.d.pre.tmp -nostdinc -I./scripts/dtc/include-prefixes -undef -D__DTS__ -x assembler-with-cpp -o arch/arm/boot/dts/.bsj-rv1126-hw-v000000.dtb.dts.tmp arch/arm/boot/dts/bsj-rv1126-hw-v000000.dts ; ./scripts/dtc/dtc -O dtb -o arch/arm/boot/dts/bsj-rv1126-hw-v000000.dtb -b 0 -iarch/arm/boot/dts/ -i./scripts/dtc/include-prefixes -@ -Wno-unit_address_vs_reg -Wno-unit_address_format -Wno-avoid_unnecessary_addr_size -Wno-alias_paths -Wno-graph_child_address -Wno-graph_port -Wno-simple_bus_reg -Wno-unique_unit_address -Wno-pci_device_reg  -d arch/arm/boot/dts/.bsj-rv1126-hw-v000000.dtb.d.dtc.tmp arch/arm/boot/dts/.bsj-rv1126-hw-v000000.dtb.dts.tmp ; cat arch/arm/boot/dts/.bsj-rv1126-hw-v000000.dtb.d.pre.tmp arch/arm/boot/dts/.bsj-rv1126-hw-v000000.dtb.d.dtc.tmp > arch/arm/boot/dts/.bsj-rv1126-hw-v000000.dtb.d

source_arch/arm/boot/dts/bsj-rv1126-hw-v000000.dtb := arch/arm/boot/dts/bsj-rv1126-hw-v000000.dts

deps_arch/arm/boot/dts/bsj-rv1126-hw-v000000.dtb := \
  arch/arm/boot/dts/rv1126_02r.dtsi \
  scripts/dtc/include-prefixes/dt-bindings/clock/rv1126-cru.h \
  scripts/dtc/include-prefixes/dt-bindings/power/rv1126-power.h \
  scripts/dtc/include-prefixes/dt-bindings/gpio/gpio.h \
  scripts/dtc/include-prefixes/dt-bindings/interrupt-controller/irq.h \
  scripts/dtc/include-prefixes/dt-bindings/interrupt-controller/arm-gic.h \
  scripts/dtc/include-prefixes/dt-bindings/pinctrl/rockchip.h \
  scripts/dtc/include-prefixes/dt-bindings/soc/rockchip,boot-mode.h \
  scripts/dtc/include-prefixes/dt-bindings/soc/rockchip-system-status.h \
  scripts/dtc/include-prefixes/dt-bindings/suspend/rockchip-rv1126.h \
  scripts/dtc/include-prefixes/dt-bindings/thermal/thermal.h \
  arch/arm/boot/dts/rv1126-dram-default-timing.dtsi \
  scripts/dtc/include-prefixes/dt-bindings/clock/rockchip-ddr.h \
  scripts/dtc/include-prefixes/dt-bindings/memory/rv1126-dram.h \
  scripts/dtc/include-prefixes/dt-bindings/memory/rockchip-dram.h \
  arch/arm/boot/dts/rv1126-pinctrl.dtsi \
  arch/arm/boot/dts/rockchip-pinconf.dtsi \
  arch/arm/boot/dts/rv1126-evb-v13.dtsi \
  arch/arm/boot/dts/rv1126-evb-v12.dtsi \
  arch/arm/boot/dts/rv1126-evb-v10.dtsi \
  scripts/dtc/include-prefixes/dt-bindings/display/drm_mipi_dsi.h \
  scripts/dtc/include-prefixes/dt-bindings/display/media-bus-format.h \
  scripts/dtc/include-prefixes/dt-bindings/input/input.h \
  scripts/dtc/include-prefixes/dt-bindings/input/linux-event-codes.h \

arch/arm/boot/dts/bsj-rv1126-hw-v000000.dtb: $(deps_arch/arm/boot/dts/bsj-rv1126-hw-v000000.dtb)

$(deps_arch/arm/boot/dts/bsj-rv1126-hw-v000000.dtb):
