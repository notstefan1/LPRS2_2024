#!/usr/bin/env julia

using Printf

function frame_precise(
	;
	addr::UInt8,
	fun::UInt8,
	reg_addr::UInt16,
	reg_cnt::UInt16,
)
	PDU = UInt8[
		fun,
		reg_addr >> 8,
		reg_addr & 0xff,
		reg_cnt >> 8,
		reg_cnt & 0xff,
	]
	addr_PDU = vcat([addr], PDU)

	LRC(addr_PDU::Vector{UInt8}) = UInt8(-sum(addr_PDU) & 0xff)

	addr_PDU_LRC = vcat(addr_PDU, [LRC(addr_PDU)])
	@show addr_PDU_LRC
	a = join([@sprintf("%02X", B) for B in addr_PDU_LRC], "")
	return ':' * a * "\r\n"
end

function frame(
	;
	addr::Int,
	fun::Int,
	reg_addr::Int,
	reg_cnt::Int,
)
	return frame_precise(
		addr = UInt8(addr),
		fun = UInt8(fun),
		reg_addr = UInt16(reg_addr),
		reg_cnt = UInt16(reg_cnt),
	)
end

@show frame(
	addr = 1,
	fun = 3,
	reg_addr = 0,
	reg_cnt = 1,
)