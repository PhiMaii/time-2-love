const dtEl = document.getElementById("dt");
const isoEl = document.getElementById("iso");
const unixEl = document.getElementById("unix");
const codePreviewEl = document.getElementById("codePreview");
const statusEl = document.getElementById("status");
const btnNow = document.getElementById("btnNow");
const btnSend = document.getElementById("btnSend");

function pad(n) {
    return String(n).padStart(2, "0");
}

/**
 * Interpret datetime-local as UTC (not local time).
 * datetime-local returns "YYYY-MM-DDTHH:mm" or with seconds.
 * We turn it into "YYYY-MM-DDTHH:mm:ssZ".
 */
function datetimeLocalToUtcIsoZ(value) {
    if (!value) return "";
    // Ensure seconds exist
    let v = value;
    if (/^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}$/.test(v)) v += ":00";
    return `${v}Z`;
}

function compute() {
    const iso = datetimeLocalToUtcIsoZ(dtEl.value);
    isoEl.value = iso;

    if (!iso) {
        unixEl.value = "";
        codePreviewEl.textContent = "";
        return;
    }

    const unix = Math.floor(new Date(iso).getTime() / 1000);
    unixEl.value = String(unix);

    codePreviewEl.textContent =
        `// Same conversion you requested:
Math.floor(new Date('${iso}').getTime() / 1000);

// Result:
${unix}`;
}

function setStatus(msg, isError = false) {
    statusEl.textContent = msg;
    statusEl.style.color = isError ? "rgba(255,120,120,0.95)" : "rgba(255,255,255,0.70)";
}

dtEl.addEventListener("input", () => {
    compute();
    setStatus("");
});

btnNow.addEventListener("click", () => {
    const now = new Date();
    const yyyy = now.getUTCFullYear();
    const mm = pad(now.getUTCMonth() + 1);
    const dd = pad(now.getUTCDate());
    const hh = pad(now.getUTCHours());
    const mi = pad(now.getUTCMinutes());
    const ss = pad(now.getUTCSeconds());

    // datetime-local doesn't accept "Z", so we set it without Z
    dtEl.value = `${yyyy}-${mm}-${dd}T${hh}:${mi}:${ss}`;
    compute();
    setStatus("Filled with current UTC time.");
});

btnSend.addEventListener("click", async () => {
    compute();

    const iso = isoEl.value;
    const unix = Number(unixEl.value);

    if (!iso || !Number.isFinite(unix)) {
        setStatus("Pick a date/time first.", true);
        return;
    }

    setStatus("Sending…");

    try {
        const res = await fetch("/setEvent/api", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ iso, unix }),
        });

        const data = await res.json().catch(() => ({}));

        if (!res.ok) {
            setStatus(`Backend error (${res.status}): ${data.error || "unknown"}`, true);
            return;
        }

        setStatus(`Sent! Backend replied: ok=${data.ok}`);
    } catch (err) {
        setStatus(`Network error: ${err?.message || err}`, true);
    }
});

// Optional: prefill with a sensible default (UTC) on first load
btnNow.click();