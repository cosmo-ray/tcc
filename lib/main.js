let glob_wasm = null;
const env = {
    show_mem() {
	let mem = glob_wasm.instance.exports.memory;
	const membuf = new Uint8Array(mem.buffer);
	console.log(membuf);
    },
    puts(ptr) {
	let mem = glob_wasm.instance.exports.memory;
	const membuf = new Uint8Array(mem.buffer);
	let end = ptr;
	while (membuf[end] !== 0) end++;
	console.log(new TextDecoder().decode(membuf.subarray(ptr, end)));
    },
    printf(ptr, vargs_ptr) {
	let mem = glob_wasm.instance.exports.memory;
	const membuf = new Uint8Array(mem.buffer);
	let end = ptr;
	while (membuf[end] !== 0) end++;
	const fmt = new TextDecoder().decode(membuf.subarray(ptr, end));
	const i32 = new Int32Array(mem.buffer);
	let out = "";
	for (let j = 0, arg = 0; j < fmt.length; j++) {
	    if (fmt[j] === '%' && j + 1 < fmt.length) {
		const c = fmt[++j];
		if (c === '%') out += '%';
		else if (c === 's') {
		    const sp = i32[(vargs_ptr + arg * 8) >> 2];
		    let e = sp; while (membuf[e] !== 0) e++;
		    out += new TextDecoder().decode(membuf.subarray(sp, e));
		    arg++;
		} else if (c === 'd' || c === 'i') {
		    out += i32[(vargs_ptr + arg * 8) >> 2];
		    arg++;
		} else out += '%' + c;
	    } else out += fmt[j];
	}
	process.stdout.write(out);
	return 0;
    },
    memset(ptr, value, size) {
	let mem = glob_wasm.instance.exports.memory;
	const membuf = new Uint8Array(mem.buffer);
	membuf.fill(value & 0xff, ptr, ptr + size);
	return ptr;
    }
};
const lib = WebAssembly.instantiate(new Uint8Array(buf), { env }).
      then(res => {
	  glob_wasm = res
	  let r = res.instance.exports.main()
	  return r;});
